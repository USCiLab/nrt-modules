#ifndef HERMES_H
#define HERMES_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>

class SerialPort; 

namespace hermes
{
  NRT_DECLARE_TRANSFORM_UPDATE_PORT( OdometryUpdatePort );
	NRT_DECLARE_MESSAGESUBSCRIBER_PORT( VelocityCommand, nrt::VelocityMessage, void, "The requested velocity of the robot" );

  static const nrt::ParameterCategory ParamCat("Hermes Related Options");

  static const nrt::ParameterDef<std::string> BaseFrameParamDef("baseframe", 
      "The name of the base frame from which the odometry frame is referenced", "world", ParamCat);

  static const nrt::ParameterDef<std::string> OdomFrameParamDef("odomframe", 
      "The name of the base frame from which the odometry frame is referenced", "odometry", ParamCat);

  static const nrt::ParameterDef<std::string> SerialDevParam("serialdev", 
      "The serial device file", "", ParamCat);
}

class HermesModule : public nrt::Module,
										 public nrt::MessageSubscriber<hermes::VelocityCommand>,
                     public nrt::MessagePoster<hermes::OdometryUpdatePort>
{
  public:
    HermesModule(std::string const& instanceName="");

    void run();

    void serialDevCallback(std::string const & dev);

		void onMessage(hermes::VelocityCommand msg);

  private:
		std::mutex itsMtx;
    std::shared_ptr<SerialPort> itsSerialPort;
    nrt::Parameter<std::string> itsOdometryBaseFrame;
    nrt::Parameter<std::string> itsOdometryOdomFrame;
    nrt::Parameter<std::string> itsSerialDev;
};

#endif // HERMES_H 
