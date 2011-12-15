#ifndef IROBOTCREATE_H
#define IROBOTCREATE_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>

class SerialPort; 

namespace irobotcreate
{
  NRT_DECLARE_TRANSFORM_UPDATE_PORT( OdometryUpdatePort );
	NRT_DECLARE_MESSAGESUBSCRIBER_PORT( VelocityCommand, nrt::VelocityMessage, void, "The requested velocity of the robot" );

  static const nrt::ParameterCategory ParamCat("iRobot Create Related Options");

  static const nrt::ParameterDef<std::string> BaseFrameParamDef("baseframe", 
      "The name of the base frame from which the odometry frame is referenced", "world", ParamCat);

  static const nrt::ParameterDef<std::string> OdomFrameParamDef("odomframe", 
      "The name of the base frame from which the odometry frame is referenced", "odometry", ParamCat);

  static const nrt::ParameterDef<std::string> SerialDevParam("serialdev", 
      "The serial device file", "", ParamCat);
}

class iRobotCreateModule : public nrt::Module,
													 public nrt::MessageSubscriber<irobotcreate::VelocityCommand>,
                           public nrt::MessagePoster<irobotcreate::OdometryUpdatePort>
{
  public:
    iRobotCreateModule(std::string const& instanceName="");

    void run();

    void serialDevCallback(std::string const & dev);

		void onMessage(irobotcreate::VelocityCommand msg);

  private:
		std::mutex itsMtx;
    std::shared_ptr<SerialPort> itsSerialPort;
    nrt::Parameter<std::string> itsOdometryBaseFrame;
    nrt::Parameter<std::string> itsOdometryOdomFrame;
    nrt::Parameter<std::string> itsSerialDev;
};

#endif // IROBOTCREATE_H 
