#include "HermesModule.H"
#include <nrt/Core/Util/MathUtils.H>
#include "Firmware/hermes/serialdata.h"
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>


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
    itsVelocityCommand =
    {
      nrt::byte(CMD_RESET),
      nrt::byte(CMD_SETSPEED),
      leftspeed,
      rightspeed,
    };
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

    auto endTime = nrt::now() + std::chrono::milliseconds(10);

    //write velocity command to hermes;
    std::vector<byte> velocityCommand;
    {
      std::lock_guard<std::mutex> _(itsVelocityCommandMtx);
      velocityCommand = itsVelocityCommand;
    }

    //if (itsLastBatteryReading < itsBatteryCutoffParam.getVal())
    //  NRT_WARNING("Hermes reports low battery! (" << itsLastBatteryReading << ") Bailing out!");


    bool reading = true;
    bool read_success = false;
    std::vector<byte> read_buffer(10);
    auto readHandler = [&](const boost::system::error_code& error, size_t bytes_transferred)
    {
      if(error && error.value() != 2) // What the hell is 2?
      {
        NRT_INFO("Read Handler Error: " << error.message() << " " << error.default_error_condition().value());
        reading      = false;
        read_success = false;
      }
      else
      {
        itsMessageBuffer.insert(itsMessageBuffer.end(), read_buffer.begin(), read_buffer.begin()+bytes_transferred);

        NRT_INFO("Read Handler:"
            << " bytes_transferred: " << bytes_transferred 
            << " read_buffer.size(): " << read_buffer.size() 
            << " itsMessageBuffer.size(): " << itsMessageBuffer.size());
      }
    };

    boost::asio::async_read(itsSerialPort, boost::asio::buffer(read_buffer, 10), readHandler);

    itsIO.reset();
    while(reading && (nrt::now() < endTime))
    {
      itsIO.run_one();
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    NRT_INFO("Done Read");


    boost::asio::write(itsSerialPort, boost::asio::buffer(velocityCommand, 4));

    std::this_thread::sleep_until(endTime);


  }


  //while (running())
  //{
    //if(itsSerialPort.IsOpen()) 
    //{ 
    //  NRT_WARNING("Serial Port Not Open");
    //  sleep(1);
    //  continue;
    //}

    //auto endTime = nrt::now() + std::chrono::milliseconds(10);

    ////read sensor readings from hermes;
    //while(itsSerialPort.IsDataAvailable() && running())
    //{
    //  unsigned char dataIn = itsSerialPort.ReadByte();
    //  if(dataIn == SEN_COMPASS)
    //  {
    //    compassPacket packet;
    //    for(int i=0; i<sizeof(compassPacket); i++)
    //    {
    //      packet.raw[i] = itsSerialPort.ReadByte();
    //    }

    //    Message<nrt::real>::unique_ptr msg(new Message<nrt::real>);
    //    msg->value = packet.heading;
    //    post<CompassZ>(msg);
    //  }
    //  else if(dataIn == SEN_GYRO) 
    //  {
    //    gyroPacket packet;
    //    for(int i=0; i<sizeof(gyroPacket); i++)
    //    {
    //      packet.raw[i] = itsSerialPort.ReadByte();
    //    }

    //    //NRT_INFO("Data: X("<<packet.xyz[0]<<") Y("<<packet.xyz[1]<<") Z("<<packet.xyz[2]<<")");
    //    Message<nrt::real>::unique_ptr msg(new Message<nrt::real>);
    //    msg->value = (NRT_D_PI/180.0) * packet.xyz[2];
    //    post<GyroZ>(msg);
    //  }
    //  else if(dataIn == SEN_BATTERY) {
    //    batteryPacket packet;
    //    for(int i=0; i<sizeof(batteryPacket); i++)
    //    {
    //      packet.raw[i] = itsSerialPort.ReadByte();
    //    }
    //    itsLastBatteryReading = packet.voltage;
    //  }
    //  else
    //    NRT_WARNING("Unrecognized data read from Hermes: " << dataIn);
    //}

    ////write velocity command to hermes;
    //std::vector<byte> velocityCommand;
    //{
    //  std::lock_guard<std::mutex> _(itsVelocityCommandMtx);
    //  velocityCommand = itsVelocityCommand;
    //}

    ////if (itsLastBatteryReading < itsBatteryCutoffParam.getVal())
    ////  NRT_WARNING("Hermes reports low battery! (" << itsLastBatteryReading << ") Bailing out!");

    //itsSerialPort.Write(velocityCommand);
    //std::this_thread::sleep_until(endTime);
  //}
  //itsSerialPort.Close();
}

NRT_REGISTER_MODULE(HermesModule);
