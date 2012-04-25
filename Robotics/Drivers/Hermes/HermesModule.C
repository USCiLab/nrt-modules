#include "HermesModule.H"
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
  itsSerialDev(SerialDevParam, this, &HermesModule::serialDevCallback),
  itsTrimConstantParam(TrimConstantParam, this)
{ }

// ######################################################################
HermesModule::~HermesModule()
{ }

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

  // 0   - full backwards
  // 64  - stopped
  // 128 - full forwards
  nrt::byte leftspeed  = std::round(nrt::clamped((transvel + rotwheeldist)*64.0/1.5, -64.0, 64.0)) + 64;
  nrt::byte rightspeed = std::round(nrt::clamped((transvel - rotwheeldist)*64.0/1.5, -64.0, 64.0)) + 64;

  leftspeed  = std::round(nrt::clamped(0.5*leftspeed + leftspeed*(1 - itsTrimConstantParam.getVal()), -64.0, 64.0));
  rightspeed = std::round(nrt::clamped(0.5*rightspeed + rightspeed*itsTrimConstantParam.getVal(), -64.0, 64.0));

  std::lock_guard<std::mutex> _(itsMtx);
  itsVelocityCommand = 
  {
    byte(CMD_RESET),
    byte(CMD_SETSPEED),
    leftspeed,
    rightspeed,
  };

  itsLastCommandTime = nrt::now();
}

// ######################################################################
void HermesModule::run()
{
  while (running())
  {
    if(!itsSerialPort) 
    { 
      sleep(1);
      continue;
    }

    auto endTime = nrt::now() + std::chrono::milliseconds(10);

    //read sensor readings from hermes;
    while(itsSerialPort->IsDataAvailable())
    {
      //NRT_INFO("Hermes says: " << itsSerialPort->ReadLine());
      unsigned char dataIn = itsSerialPort->ReadByte();
      if(dataIn == SEN_COMPASS)
      {
        compassPacket packet;
        for(int i=0; i<sizeof(compassPacket); i++)
        {
          packet.raw[i] = itsSerialPort->ReadByte();
        }

        Message<nrt::real>::unique_ptr msg(new Message<nrt::real>);
        msg->value = packet.heading;
        post<CompassZ>(msg);
      }
      else if(dataIn == SEN_GYRO) 
      {
        gyroPacket packet;
        for(int i=0; i<sizeof(gyroPacket); i++)
        {
          packet.raw[i] = itsSerialPort->ReadByte();
        }
        
        //NRT_INFO("Data: X("<<packet.xyz[0]<<") Y("<<packet.xyz[1]<<") Z("<<packet.xyz[2]<<")");
        Message<nrt::real>::unique_ptr msg(new Message<nrt::real>);
        msg->value = (NRT_D_PI/180.0) * packet.xyz[2];
        post<GyroZ>(msg);
      }
      else if(dataIn == SEN_BATTERY) {
        batteryPacket packet;
        for(int i=0; i<sizeof(batteryPacket); i++)
        {
          packet.raw[i] = itsSerialPort->ReadByte();
        }
        //NRT_INFO("Battery Level: " << packet.voltage << " Volts");
      }
      else
        NRT_WARNING("Unrecognized data read from Hermes: " << dataIn);
    }

    //write velocity command to hermes;
    std::string velocityCommand;
    {
      std::lock_guard<std::mutex> _(itsMtx);
      velocityCommand = itsVelocityCommand;
    }
    itsSerialPort->Write(velocityCommand);
    std::this_thread::sleep_until(endTime);
  }
}

NRT_REGISTER_MODULE(HermesModule);
