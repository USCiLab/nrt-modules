#include "iRobotCreate.H"
#include <SerialPort.h>
#include <nrt/Core/Util/MathUtils.H>

using namespace nrt;
using namespace irobotcreate;

// ######################################################################
iRobotCreateModule::iRobotCreateModule(std::string const& instanceName) :
  Module(instanceName),
  itsOdometryBaseFrame(BaseFrameParamDef, this),
  itsOdometryOdomFrame(OdomFrameParamDef, this),
  itsSerialDev(SerialDevParam, this, &iRobotCreateModule::serialDevCallback)
{ }

// ######################################################################
void iRobotCreateModule::serialDevCallback(std::string const & dev)
{
	std::lock_guard<std::mutex> lock(itsMtx);

	if(itsSerialPort) itsSerialPort.reset();
	if(dev == "") return;

	try
	{
		itsSerialPort = std::make_shared<SerialPort>(dev);
		itsSerialPort->Open(SerialPort::BAUD_57600);
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

	// Send the OI start command
	itsSerialPort->WriteByte(128);

	// Put the iRobot into full command mode
	itsSerialPort->WriteByte(132);

}

// ######################################################################
void iRobotCreateModule::onMessage(VelocityCommand msg)
{
	// 1/2 the wheelbase of the create
	double const radius = .258/2.0; 

	double const transvel = msg->linear.x();
	double const rotvel   = msg->angular.z();

	// The distance each wheel should travel to accomodate the rotational velocity
	double const rotwheeldist = rotvel * radius; 

	int const leftspeed  = std::round(nrt::clamped((transvel + rotwheeldist)/1000.0, -500.0, 500.0));
	int const rightspeed = std::round(nrt::clamped((transvel - rotwheeldist)/1000.0, -500.0, 500.0));

	std::string command = 
	{
		nrt::byte(145),
		(rightspeed >> 8) && 0x00FF,
		(rightspeed >> 0) && 0x00FF,
		(leftspeed >> 8) && 0x00FF,
		(leftspeed >> 0) && 0x00FF
	};

	{
		std::lock_guard<std::mutex> lock(itsMtx);
		itsSerialPort->Write(command);
	}
}

// ######################################################################
void iRobotCreateModule::run()
{
  while(running())
  {
		std::lock_guard<std::mutex> lock(itsMtx);
		NRT_INFO("I'm Awesome");
		usleep(100000);
  }
}

NRT_REGISTER_MODULE(iRobotCreateModule);

