#include "HermesModule.H"
#include <nrt/Core/Util/MathUtils.H>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace nrt;
using namespace hermes; 
using namespace boost::asio;

// ######################################################################
HermesModule::HermesModule(std::string const& instanceName) :
  Module(instanceName),
  itsSerialPort(itsIO),
  itsOdometryBaseFrame(BaseFrameParamDef, this),
  itsOdometryOdomFrame(OdomFrameParamDef, this),
  itsSerialDev(SerialDevParam, this, &HermesModule::serialDevCallback),
  itsBatteryCutoffParam(BatteryCutoffParam, this),
  itsTrimConstantParam(TrimConstantParam, this)
{
  itsVelocityCommand.raw[0] = 0;
  itsVelocityCommand.raw[1] = 0;
}

// ######################################################################
HermesModule::~HermesModule()
{
  if(itsSerialPort.is_open())
    itsSerialPort.close();
}

// ######################################################################
void HermesModule::serialDevCallback(std::string const & dev)
{
  std::lock_guard<std::mutex> _(itsSerialPortMtx);

  if(dev == "") return;

  if(itsSerialPort.is_open()) itsSerialPort.close();

  try
  {
    itsSerialPort.open(dev);
    itsSerialPort.set_option(serial_port_base::baud_rate(9600));
    itsSerialPort.set_option(serial_port_base::character_size( 8 ));
    itsSerialPort.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
    itsSerialPort.set_option(serial_port_base::parity(serial_port_base::parity::none));
    itsSerialPort.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
  }
  catch(boost::system::system_error e)
  {
    throw exception::BadParameter(std::string("Open Failed: ") + e.what());
  }
}

// ######################################################################
void HermesModule::onMessage(VelocityCommand msg)
{
  // 1/2 the wheelbase of the create
  double const radius = 0.3429/2.0; 

  double const transvel = msg->linear.x();
  double const rotvel   = msg->angular.z();

  // The distance each wheel should travel to accomodate the rotational velocity
  double const rotwheeldist = rotvel * radius; 

  // 0   - full backwards
  // 64  - stopped
  // 128 - full forwards
  double const lefRaw  = (transvel+rotwheeldist) * (1 - itsTrimConstantParam.getVal() + 0.5);
  double const rigRaw  = (transvel-rotwheeldist) * (0 + itsTrimConstantParam.getVal() + 0.5);

  nrt::byte const leftspeed  = std::round(nrt::clamped((lefRaw)*64.0/1.5, -64.0, 64.0)) + 64;
  nrt::byte const rightspeed = std::round(nrt::clamped((rigRaw)*64.0/1.5, -64.0, 64.0)) + 64;

  NRT_INFO("Setting velocity to " << leftspeed << ", " << rightspeed);
  {
    std::lock_guard<std::mutex> _(itsVelocityCommandMtx);
    itsVelocityCommand.values.left = leftspeed;
    itsVelocityCommand.values.right = rightspeed;
  }
}

// ######################################################################
std::vector<byte> HermesModule::serialRead(const int ms, const size_t length)
{
  std::vector<byte> read_buffer(length);
  std::vector<byte> retn_buffer(length);
  bool readComplete = false, timedOut = false;
  int bytesRead = 0;

  auto finishedRead = [&](const boost::system::error_code & ec, const size_t bytes_transferred)
  {
    if (ec.value() != 0)
    {
      if (ec.value() != boost::asio::error::operation_aborted)
        NRT_WARNING("Read had some kind of error: " << ec.value());
      readComplete = false;
    }
    else
    {
      readComplete = true;
      bytesRead = bytes_transferred;
    }
  };

  auto finishedTimer = [&](const boost::system::error_code & ec)
  {
    if (ec.value() != 0)
    {
      if (ec.value() != boost::asio::error::operation_aborted)
        NRT_WARNING("Timer had some kind of error: " << ec.value());
      timedOut = false;
    }
    else
    {
      timedOut = true;
    }
  };

  deadline_timer timer( itsIO );
  timer.expires_from_now(boost::posix_time::millisec(ms));
  timer.async_wait( finishedTimer );

  itsSerialPort.async_read_some( boost::asio::buffer(read_buffer), finishedRead );
  itsIO.reset();

  while (itsIO.run_one())
  {
    if (timedOut)
      itsSerialPort.cancel();
    else if (readComplete)
      timer.cancel();
    
    retn_buffer.insert(retn_buffer.end(), read_buffer.begin(), read_buffer.begin() + bytesRead);
  }
  return retn_buffer;
}

// ######################################################################
void HermesModule::processMessageBuffer()
{
  static std::vector<byte> dataCodes = {SEN_BATTERY, SEN_COMPASS, SEN_GYRO};

  while (itsMessageBuffer.size() > 0)
  {
    std::vector<byte>::iterator firstCode =
      std::find_first_of(itsMessageBuffer.begin(), itsMessageBuffer.end(), dataCodes.begin(), dataCodes.end());

    itsMessageBuffer.erase(itsMessageBuffer.begin(), firstCode);

    if (itsMessageBuffer[0] == SEN_BATTERY)
    {
      if (itsMessageBuffer.size() < sizeof(batteryPacket)+2)
        return;

      batteryPacket packet;
      std::copy(itsMessageBuffer.begin()+1, itsMessageBuffer.begin() + sizeof(batteryPacket)+1, &packet.raw[0]);
      byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(batteryPacket), 0, std::bit_xor<byte>());

      if(itsMessageBuffer[sizeof(batteryPacket)+1] == checksum)
      {
        itsLastBatteryReading = packet.voltage;
        NRT_INFO("Got Battery: " << packet.voltage);

        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(batteryPacket) + 2);
      }
      else
      {
        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin()+1);
      }
    }
    else if (itsMessageBuffer[0] == SEN_COMPASS)
    {
      if (itsMessageBuffer.size() < sizeof(compassPacket)+2)
        return;

      compassPacket packet;
      std::copy(itsMessageBuffer.begin()+1, itsMessageBuffer.begin() + sizeof(compassPacket)+1, &packet.raw[0]);
      byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(compassPacket), 0, std::bit_xor<byte>());

      if(itsMessageBuffer[sizeof(compassPacket)+1] == checksum)
      {
        NRT_INFO("Got Compass heading: " << packet.heading);
        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(compassPacket) + 2);
      }
      else
      {
        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin()+1);
      }
    }
    else if (itsMessageBuffer[0] == SEN_GYRO)
    {
      if (itsMessageBuffer.size() < sizeof(gyroPacket)+2)
        return;

      gyroPacket packet;
      std::copy(itsMessageBuffer.begin()+1, itsMessageBuffer.begin() + sizeof(gyroPacket)+1, &packet.raw[0]);
      byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(gyroPacket), 0, std::bit_xor<byte>());

      if(itsMessageBuffer[sizeof(gyroPacket)+1] == checksum)
      {
        NRT_INFO("Got gyro: " << packet.xyz[2]);
        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(gyroPacket) + 2);
      }
      else
      {
        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin()+1);
      }
    }
  }
}

// ######################################################################
void HermesModule::run()
{
  while(running())
  {
    if(!itsSerialPort.is_open())
    {
      NRT_WARNING("Serial Port is Not Open");
      sleep(1);
      continue;
    }

    auto endTime = nrt::now() + std::chrono::milliseconds(100);

    std::vector<byte> data = serialRead(10, 20);
    itsMessageBuffer.insert(itsMessageBuffer.end(), data.begin(), data.end());

    processMessageBuffer();
    
    // write velocity command to hermes;
    std::vector<byte> velocityCommand = {CMD_RESET, CMD_SETSPEED};
    {
      std::lock_guard<std::mutex> _(itsVelocityCommandMtx);
      velocityCommand.push_back(itsVelocityCommand.values.left);
      velocityCommand.push_back(itsVelocityCommand.values.right);
    }

    boost::asio::write(itsSerialPort, boost::asio::buffer(velocityCommand));
    std::this_thread::sleep_until(endTime);
  }
}

NRT_REGISTER_MODULE(HermesModule);
