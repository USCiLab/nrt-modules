#include "iRobotCreate.H"
#include <SerialPort.h>

using namespace nrt;

// ######################################################################
iRobotCreateModule::iRobotCreateModule(std::string const& instanceName) :
  Module(instanceName),
  itsOdometryBaseFrame(irobotcreate::BaseFrameParamDef, this),
  itsOdometryOdomFrame(irobotcreate::OdomFrameParamDef, this),
  itsSerialDev(irobotcreate::SerialDevParam, this, &iRobotCreateModule::serialDevCallback)
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

	itsSerialPort->Open(SerialPort::BAUD_115200);
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



