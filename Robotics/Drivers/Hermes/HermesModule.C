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

    auto endTime = nrt::now() + std::chrono::milliseconds(100);

    //write velocity command to hermes;
    std::vector<byte> velocityCommand;
    {
      std::lock_guard<std::mutex> _(itsVelocityCommandMtx);
      velocityCommand = itsVelocityCommand;
    }

    //if (itsLastBatteryReading < itsBatteryCutoffParam.getVal())
    //  NRT_WARNING("Hermes reports low battery! (" << itsLastBatteryReading << ") Bailing out!");


    bool reading = true;
    std::vector<byte> read_buffer(10);
    auto readHandler = [&](const boost::system::error_code& error, size_t bytes_transferred)
    {
      if(error && error.value() != 2) // What the hell is 2?
      {
        //NRT_INFO("Read Handler Error: " << error.message() << " " << error.default_error_condition().value());
        reading = false;
      }
      else
      {
        itsMessageBuffer.insert(itsMessageBuffer.end(), read_buffer.begin(), read_buffer.begin()+bytes_transferred);
        NRT_INFO("Message Buffer Size: " << itsMessageBuffer.size());

        while(itsMessageBuffer.size() && nrt::now() < endTime)
        {
          while(itsMessageBuffer.size() 
              //&& itsMessageBuffer[0] != SEN_COMPASS
              //&& itsMessageBuffer[0] != SEN_GYRO
              && itsMessageBuffer[0] != SEN_BATTERY)
            itsMessageBuffer.erase(itsMessageBuffer.begin());

          if(itsMessageBuffer.size() == 0) return;

          //if(itsMessageBuffer[0] == SEN_COMPASS && itsMessageBuffer.size() >= sizeof(compassPacket) + 1)
          //{
          //  compassPacket packet;
          //  std::copy(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(compassPacket), &packet.raw[0]);
          //  byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(compassPacket), 0, std::bit_xor<byte>());
          //  
          //  if(itsMessageBuffer[sizeof(compassPacket)] == checksum)
          //  {
          //    Message<nrt::real>::unique_ptr msg(new Message<nrt::real>);
          //    msg->value = packet.heading;
          //    post<CompassZ>(msg);
          //    NRT_INFO("Got Compass: " << packet.heading);
          //    itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(compassPacket) + 2);
          //  }
          //  else
          //  {
          //    NRT_WARNING("Bad Checksum");
          //    for(int i=0; i<=sizeof(compassPacket)+1; ++i)
          //      std::cout << (int)itsMessageBuffer[i] << " ";
          //    std::cout << std::endl;
          //    itsMessageBuffer.erase(itsMessageBuffer.begin());
          //  }
          //}
          //else if(itsMessageBuffer[0] == SEN_GYRO && itsMessageBuffer.size() >= sizeof(gyroPacket) + 1)
          //{
          //  gyroPacket packet;
          //  std::copy(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(gyroPacket), &packet.raw[0]);
          //  byte checksum = std::accumulate(&packet.raw[0], &packet.raw[0] + sizeof(gyroPacket), 0, std::bit_xor<byte>());

          //  if(itsMessageBuffer[sizeof(gyroPacket)] == checksum)
          //  {
          //    Message<nrt::real>::unique_ptr msg(new Message<nrt::real>);
          //    msg->value = (NRT_D_PI/180.0) * packet.xyz[2];
          //    post<GyroZ>(msg);
          //    NRT_INFO("Got Gyro: " << packet.xyz[2]);
          //    itsMessageBuffer.erase(itsMessageBuffer.begin(), itsMessageBuffer.begin() + sizeof(gyroPacket) + 1);
          //  }
          //  else
          //  {
          //    NRT_WARNING("Bad Checksum");
          //    for(int i=0; i<=sizeof(gyroPacket); ++i)
          //      std::cout << (int)itsMessageBuffer[i] << " ";
          //    std::cout << std::endl;
          //    itsMessageBuffer.erase(itsMessageBuffer.begin());
          //  }
          //}
          if(itsMessageBuffer[0] == SEN_BATTERY && itsMessageBuffer.size() >= sizeof(batteryPacket) + 2)
          {
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
              NRT_WARNING("Bad Checksum");
              for(int i=0; i<sizeof(batteryPacket)+2; ++i)
                std::cout << (int)itsMessageBuffer[i] << " ";
              std::cout << std::endl;
              itsMessageBuffer.erase(itsMessageBuffer.begin());
            }
          }
        }
        reading = false;
      }
    };

    boost::asio::async_read(itsSerialPort, boost::asio::buffer(read_buffer, 10), readHandler);

    itsIO.reset();
    while(reading && (nrt::now() < endTime))
    {
      itsIO.poll();
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    //itsSerialPort.cancel();

    boost::asio::write(itsSerialPort, boost::asio::buffer(velocityCommand, 4));

    std::this_thread::sleep_until(endTime);
  }
}

NRT_REGISTER_MODULE(HermesModule);
