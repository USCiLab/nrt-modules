#include "Hermes.H"
#include <SerialPort.h>
#include <nrt/Core/Util/MathUtils.H>

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
  itsSerialPort->WriteByte(97);
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

  std::string command = 
  {
    byte(98),
    leftspeed,
    rightspeed,
  };


  {
    std::lock_guard<std::mutex> lock(itsMtx);
    if(!itsSerialPort) return;

    NRT_INFO("Sending Serial Port Command");
    itsSerialPort->Write(command);
  }
  NRT_INFO("Sent Velocity [" << leftspeed << "," << rightspeed << "]");
}

// ######################################################################
void HermesModule::run()
{
  //while(running())
  //{
  //	//std::lock_guard<std::mutex> lock(itsMtx);
  //	//if(!itsSerialPort) return;

  //	//NRT_INFO("I'm Awesome");
  //	//sleep(1);
  //}
}

NRT_REGISTER_MODULE(HermesModule);

