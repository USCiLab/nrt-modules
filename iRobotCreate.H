#ifndef IROBOTCREATE_H
#define IROBOTCREATE_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>

class SerialPort; 

namespace irobotcreate
{
  NRT_DECLARE_TRANSFORM_UPDATE_PORT( OdometryUpdatePort );

  static const nrt::ParameterCategory ParamCat("iRobot Create Related Options");

  static const nrt::ParameterDef<std::string> BaseFrameParamDef("baseframe", 
      "The name of the base frame from which the odometry frame is referenced", "world", ParamCat);

  static const nrt::ParameterDef<std::string> OdomFrameParamDef("odomframe", 
      "The name of the base frame from which the odometry frame is referenced", "odometry", ParamCat);

  static const nrt::ParameterDef<std::string> SerialDevParam("serialdev", 
      "The serial device file", "", ParamCat);
}

class iRobotCreateModule : public nrt::Module,
                           public nrt::MessagePoster<irobotcreate::OdometryUpdatePort>
{
  public:
    iRobotCreateModule(std::string const& instanceName="");

    void run();

    void serialDevCallback(std::string const & dev);

  private:
		std::mutex itsMtx;
    std::shared_ptr<SerialPort> itsSerialPort;
    nrt::Parameter<std::string> itsOdometryBaseFrame;
    nrt::Parameter<std::string> itsOdometryOdomFrame;
    nrt::Parameter<std::string> itsSerialDev;
};

#endif // IROBOTCREATE_H 
