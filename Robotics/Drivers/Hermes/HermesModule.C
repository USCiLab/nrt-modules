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
    itsVelocityCommand.left = leftspeed;
    itsVelocityCommand.right = rightspeed;
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
    std::vector<byte>::iterator startByte = std::find(itsMessageBuffer.begin(), itsMessageBuffer.end(), 255);

    if (startByte != itsMessageBuffer.end())
      itsMessageBuffer.erase(itsMessageBuffer.begin(), startByte+1);

    if (itsMessageBuffer.size() == 0)
      continue;

    if (itsMessageBuffer[0] == SEN_BATTERY)
    {
      if (itsMessageBuffer.size() < sizeof(BatteryPacket)+2)
        return;

      BatteryPacket packet;
      std::copy(itsMessageBuffer.begin()+1, itsMessageBuffer.begin() + sizeof(BatteryPacket)+1, &packet.raw[0]);
      byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(BatteryPacket), 255^SEN_BATTERY, std::bit_xor<byte>());

      if(itsMessageBuffer[sizeof(BatteryPacket)+1] == checksum)
      {
        itsLastBatteryReading = packet.voltage;
        NRT_INFO("Got Battery: " << packet.voltage);

        std::unique_ptr<nrt::Message<nrt::real>> msg(new nrt::Message<nrt::real>(packet.voltage));
        post<hermes::Battery>(msg);

        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(BatteryPacket) + 2);
      }
      else
      {
        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin()+1);
      }
    }
    else if (itsMessageBuffer[0] == SEN_COMPASS)
    {
      if (itsMessageBuffer.size() < sizeof(CompassPacket)+2)
        return;

      CompassPacket packet;
      std::copy(itsMessageBuffer.begin()+1, itsMessageBuffer.begin() + sizeof(CompassPacket)+1, &packet.raw[0]);
      byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(CompassPacket), 255^SEN_BATTERY, std::bit_xor<byte>());

      if(itsMessageBuffer[sizeof(CompassPacket)+1] == checksum)
      {
        NRT_INFO("Got Compass heading: " << packet.heading);

        std::unique_ptr<nrt::Message<nrt::real>> msg(new nrt::Message<nrt::real>(packet.heading));
        post<hermes::CompassZ>(msg);
        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(CompassPacket) + 2);
      }
      else
      {
        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin()+1);
      }
    }
    else if (itsMessageBuffer[0] == SEN_GYRO)
    {
      if (itsMessageBuffer.size() < sizeof(GyroPacket)+2)
        return;

      GyroPacket packet;
      std::copy(itsMessageBuffer.begin()+1, itsMessageBuffer.begin() + sizeof(GyroPacket)+1, &packet.raw[0]);
      byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(GyroPacket), 255^SEN_GYRO, std::bit_xor<byte>());

      if(itsMessageBuffer[sizeof(GyroPacket)+1] == checksum)
      {
        NRT_INFO("Got gyro: " << packet.y);

        std::unique_ptr<nrt::Message<nrt::real>> msg(new nrt::Message<nrt::real>(packet.y));
        post<hermes::GyroZ>(msg);

        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(GyroPacket) + 2);
      }
      else
      {
        itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin()+1);
      }
    }
  }
}

void HermesModule::writePacket(CommandPacket packet)
{
  byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(CommandPacket), 255, std::bit_xor<byte>());
  vector<byte> data = {255, packet.command, packet.data1, packet.data2, checksum};

  boost::asio::write(itsSerialPort, boost::asio::buffer(data));

  vector<byte> buf = serialRead(1000, 7);
  checksum = std::accumulate(); // TODO
  
  if (buf[0] != 255)
    throw exception::BadParameter("Bad start byte.");

  if (buf[1] != data[1])
    throw exception::BadParameter("Got wrong packet back.");

  if 

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
    byte checksum = std::accumulate(&itsVelocityCommand.raw[0], &itsVelocityCommand.raw[0] + sizeof(MotorPacket), 255, std::bit_xor<byte>());
    std::vector<byte> velocityCommand = {255};
    {
      std::lock_guard<std::mutex> _(itsVelocityCommandMtx);
      velocityCommand.push_back(itsVelocityCommand.left);
      velocityCommand.push_back(itsVelocityCommand.right);
      velocityCommand.push_back(checksum);
    }

    boost::asio::write(itsSerialPort, boost::asio::buffer(velocityCommand));
    std::this_thread::sleep_until(endTime);
  }
}

NRT_REGISTER_MODULE(HermesModule);
