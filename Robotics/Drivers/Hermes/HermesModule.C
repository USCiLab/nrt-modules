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
  itsTrimConstantParam(TrimConstantParam, this),
  itsForwardMaxParam(ForwardMaxParam, this),
  itsBackwardMaxParam(BackwardMaxParam, this),
  itsTurnRadiusParam(TurnRadiusParam, this)
{
  itsVelocityCommand.command = packetid::ID_MOTOR;
  itsVelocityCommand.data1 = 128;
  itsVelocityCommand.data2 = 128;
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
    itsSerialPort.set_option(serial_port_base::baud_rate(115200));
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
  double const transvel = msg->linear.x();
  double const rotvel   = msg->angular.z();

  // The distance each wheel should travel to accomodate the rotational velocity
  double const rotwheeldist = 2*M_PI * rotvel * itsTurnRadiusParam.getVal(); 

  // 0   - full backwards
  // 64  - stopped
  // 128 - full forwards
  double const lefRaw  = (transvel+rotwheeldist) * (1 - itsTrimConstantParam.getVal() + 0.5);
  double const rigRaw  = (transvel-rotwheeldist) * (0 + itsTrimConstantParam.getVal() + 0.5);

  nrt::byte leftspeed; 
  nrt::byte rightspeed; 

  if (lefRaw >= 0)
    leftspeed = std::round(nrt::clamped( int(lefRaw*127/itsForwardMaxParam.getVal()), 0, 127 )) + 128;
  else
    leftspeed = 127-std::round(nrt::clamped( int(-lefRaw*127/itsBackwardMaxParam.getVal()), 0, 127 ));

  if (rigRaw >= 0)
    rightspeed = std::round(nrt::clamped( int(rigRaw*127/itsForwardMaxParam.getVal()), 0, 127 )) + 128;
  else
    rightspeed = 127-std::round(nrt::clamped( int(-rigRaw*127/itsBackwardMaxParam.getVal()), 0, 127 ));


  {
    std::lock_guard<std::mutex> _(itsVelocityCommandMtx);
    itsVelocityCommand.command = packetid::ID_MOTOR;
    itsVelocityCommand.data2 = leftspeed;
    itsVelocityCommand.data1 = rightspeed;
  }
}

// ######################################################################
std::vector<byte> HermesModule::serialRead(const int ms, const size_t length)
{
  std::vector<byte> read_buffer(length);
  std::vector<byte> retn_buffer;
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

  boost::asio::async_read(itsSerialPort, boost::asio::buffer(read_buffer, length), finishedRead);
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
ResponsePacket HermesModule::writePacket(CommandPacket packet)
{
  byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(CommandPacket), 255, std::bit_xor<byte>());
  std::vector<byte> data = {255, packet.command, packet.data1, packet.data2, checksum};
  boost::asio::write(itsSerialPort, boost::asio::buffer(data));

  std::vector<byte> buf;
  bool found = false;
  auto endTime = nrt::now() + std::chrono::milliseconds(1000);
  while (nrt::now() < endTime && !found)
  {
    std::vector<byte> tmp = serialRead(100, 1);
    if (tmp.size() > 0)
    {
      buf.push_back(tmp[0]);

      if (buf.size() > 7)
        buf.erase(buf.begin());

      if (buf.size() == 7 && buf[0] == 255 && buf[1] == data[1] && buf[6] == std::accumulate(&buf[0], &buf[0]+6, 0, std::bit_xor<byte>()))
        found = true;
    }
  }

  ResponsePacket response;
  if (!found)
  {
    NRT_WARNING("Timed out");
    response.data = -1;
    return response;
  }
  else
  {
    for (int i = 0; i < sizeof(ResponsePacket); i++)
      response.raw[i] = buf[i+2];
    return response;
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

    // send a motor command
    {
      std::lock_guard<std::mutex> _(itsVelocityCommandMtx);
      writePacket(itsVelocityCommand);
    }

    // battery
    CommandPacket cmd;
    cmd.command = packetid::ID_BATTERY;
    ResponsePacket res = writePacket(cmd);
    if (res.data != -1)
    {
      std::unique_ptr<nrt::Message<nrt::real>> msg(new nrt::Message<nrt::real>(res.data));
      post<hermes::Battery>(msg);
    }

    // magnetometer
    //cmd.command = packetid::ID_MAG_X;
    //res = writePacket(cmd);
    //NRT_INFO("MagX: " << res.data);

    //cmd.command = packetid::ID_MAG_Y;
    //res = writePacket(cmd);
    //NRT_INFO("MagY: " << res.data);

    cmd.command = packetid::ID_MAG_Z;
    res = writePacket(cmd);
    if (res.data != -1)
    {
      std::unique_ptr<nrt::Message<nrt::real>> msg(new nrt::Message<nrt::real>(res.data));
      post<hermes::CompassZ>(msg);
    }

    // gyroscope
    //cmd.command = packetid::ID_GYRO_X;
    //res = writePacket(cmd);
    //NRT_INFO("GyroX: " << res.data);

    cmd.command = packetid::ID_GYRO_Y;
    res = writePacket(cmd);
    if (res.data != -1)
    {
      std::unique_ptr<nrt::Message<nrt::real>> msg(new nrt::Message<nrt::real>(res.data));
      post<hermes::GyroZ>(msg);
    }

    //cmd.command = packetid::ID_GYRO_Z;
    //res = writePacket(cmd);
    //NRT_INFO("GyroZ:" << res.data);

    std::this_thread::sleep_until(endTime);
  }
}

NRT_REGISTER_MODULE(HermesModule);
