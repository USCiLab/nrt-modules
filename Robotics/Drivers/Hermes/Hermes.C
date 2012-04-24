#include "Hermes.H"
#include <SerialPort.h>
#include <nrt/Core/Util/MathUtils.H>
#include "Firmware/hermes/serialdata.h"

using namespace nrt;
using namespace hermes; 

// ######################################################################
HermesModule::HermesModule(std::string const& instanceName) :
  Module(instanceName),
  itsSerialPort(nullptr),
  itsOdometryBaseFrame(BaseFrameParamDef, this),
  itsOdometryOdomFrame(OdomFrameParamDef, this),
  itsSerialDev(SerialDevParam, this, &HermesModule::serialDevCallback)
{ }

// ######################################################################
HermesModule::~HermesModule()
{
  if(itsReadThread.joinable())  itsReadThread.join();
  if(itsWriteThread.joinable()) itsWriteThread.join();
}

// ######################################################################
void HermesModule::serialDevCallback(std::string const & dev)
{
  std::lock_guard<std::mutex> lock(itsMtx);

  if(itsSerialPort) itsSerialPort.reset();
  if(dev == "") return;

  try
  {
    itsSerialPort = std::make_shared<SerialPort>(dev);
    itsSerialPort->Open(
        SerialPort::BAUD_115200,
        SerialPort::CHAR_SIZE_8,
        SerialPort::PARITY_NONE,
        SerialPort::STOP_BITS_1,
        SerialPort::FLOW_CONTROL_NONE);
  }
  catch(SerialPort::AlreadyOpen & e)
  {
    // If it's already open, then who cares...
  }
  catch(SerialPort::OpenFailed & e)
  {
    throw exception::BadParameter(std::string("Open Failed: ") + e.what());
  }
  catch(std::invalid_argument & e)
  {
    throw exception::BadParameter(std::string("Invalid Argument: ") + e.what());
  }

  if(!itsSerialPort->IsOpen()) throw exception::BadParameter("Failed to open serial port");

  // Send the Hermes "reset" command 
  itsSerialPort->WriteByte(CMD_RESET);
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

  // for now assume that the fastest that Hermes can go is 11m/s
  // until we have good PID and IMU data we can't do much with actual velocity

  // 0   - 11m/s backwards
  // 64  - stopped
  // 128 - 11m/s forwards
  nrt::byte const leftspeed  = std::round(nrt::clamped((transvel + rotwheeldist)*64.0/1.5, -64.0, 64.0)) + 64;
  nrt::byte const rightspeed = std::round(nrt::clamped((transvel - rotwheeldist)*64.0/1.5, -64.0, 64.0)) + 64;

  //NRT_INFO("Got Velocity Message [" << leftspeed << " , " << rightspeed << "]");
  std::lock_guard<std::mutex> _(itsMtx);
  itsVelocityCommand = 
  {
    byte(CMD_RESET),
    byte(CMD_SETSPEED),
    leftspeed,
    rightspeed,
  };
  //NRT_INFO("Set Velocity Command [" << itsVelocityCommand << "]");
}

// ######################################################################
void HermesModule::readThreadMethod()
{
  NRT_INFO("Read Thread Started");
  while(running())
  {
    std::lock_guard<std::mutex> _(itsMtx);
    if(!itsSerialPort) 
    { 
      sleep(1);
      continue;
    }

    while(itsSerialPort->IsDataAvailable())
    {
      NRT_INFO("Data Available - Reading Byte");
      unsigned char dataIn = itsSerialPort->ReadByte();
      if(dataIn == SEN_COMPASS)
      {
        //NRT_INFO("Compass: " << dataIn);
        compassPacket packet;
        for(int i=0; i<sizeof(compassPacket); i++)
        {
          packet.raw[i] = itsSerialPort->ReadByte();
        }
        //NRT_INFO("Data: " << packet.heading);         
        Message<nrt::real>::unique_ptr msg(new Message<nrt::real>);
        msg->value = packet.heading;
        post<CompassZ>(msg);
      }
      else if(dataIn == SEN_GYRO) 
      {
        //NRT_INFO("Gyro: " << dataIn);
        gyroPacket packet;
        for(int i=0; i<sizeof(gyroPacket); i++)
        {
          packet.raw[i] = itsSerialPort->ReadByte();
        }
        //NRT_INFO("Data: X("<<packet.xyz[0]<<") Y("<<packet.xyz[1]<<") Z("<<packet.xyz[2]<<")");
        Message<nrt::real>::unique_ptr msg(new Message<nrt::real>);
        msg->value = (NRT_D_PI/180.0) * packet.xyz[2];
        post<GyroZ>(msg);
      } else if(dataIn == SEN_BATTERY) {
        batteryPacket packet;
        for(int i=0; i<sizeof(batteryPacket); i++)
        {
          packet.raw[i] = itsSerialPort->ReadByte();
        }
        NRT_INFO("Battery Level: " << packet.voltage << " Volts");
      }
      else NRT_INFO("Unrecognized: " << dataIn);
    }
  }
}

// ######################################################################
void HermesModule::writeThreadMethod()
{
  NRT_INFO("Write Thread Started");
  while(running())
  {
    std::lock_guard<std::mutex> _(itsMtx);
    if(!itsSerialPort) 
    { 
      NRT_WARNING("Hermes link not opened...");
      sleep(1);
      continue;
    }

    NRT_INFO("Sending Velocity Command: [" << itsVelocityCommand << "]");
    itsSerialPort->Write(itsVelocityCommand);
    usleep(10000);
  }
}

// ######################################################################
void HermesModule::run()
{
  if(itsReadThread.joinable())  itsReadThread.join();
  if(itsWriteThread.joinable()) itsWriteThread.join();

  itsReadThread  = std::thread(std::bind(&HermesModule::readThreadMethod, this));
  itsWriteThread = std::thread(std::bind(&HermesModule::writeThreadMethod, this));
}

NRT_REGISTER_MODULE(HermesModule);

