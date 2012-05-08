#ifndef HERMES_H
#define HERMES_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>
#include <boost/asio/serial_port.hpp>
#include "Firmware/hermes/hermesGlobals.h"

namespace hermes
{
  NRT_DECLARE_TRANSFORM_UPDATE_PORT( OdometryUpdatePort );
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT( VelocityCommand, nrt::VelocityMessage, void, "The requested velocity of the robot" );
  NRT_DECLARE_MESSAGEPOSTER_PORT( Battery, nrt::Message<nrt::real>, void, "The current battery level." );
  NRT_DECLARE_MESSAGEPOSTER_PORT( CompassZ, nrt::Message<nrt::real>, void, "The Z component of Hermes' compass" );
  NRT_DECLARE_MESSAGEPOSTER_PORT( GyroZ, nrt::Message<nrt::real>, void, "The Z component of Hermes' gyroscope" );

  static const nrt::ParameterCategory ParamCat("Hermes Related Options");

  static const nrt::ParameterDef<std::string> BaseFrameParamDef("baseframe", 
      "The name of the base frame from which the odometry frame is referenced", "world", ParamCat);
  static const nrt::ParameterDef<std::string> OdomFrameParamDef("odomframe", 
      "The name of the base frame from which the odometry frame is referenced", "odometry", ParamCat);
  static const nrt::ParameterDef<std::string> SerialDevParam("serialdev", 
      "The serial device file", "", ParamCat);
  static const nrt::ParameterDef<nrt::real> TrimConstantParam("trim",
      "A constant to adjust for drift between the left and right motors", 0.5215, ParamCat);
  static const nrt::ParameterDef<nrt::real> BatteryCutoffParam("batteryCutoff",
      "The voltage below which Hermes will refuse to drive the motors.", 13.0, ParamCat);
  static const nrt::ParameterDef<nrt::real> ForwardMaxParam("forwardMax",
      "The max forward velocity of the robot. (m/s)", 5.0, ParamCat);
  static const nrt::ParameterDef<nrt::real> BackwardMaxParam("backwardMax",
      "The max backward velocity of the robot. (m/s)", 4.0, ParamCat);
  static const nrt::ParameterDef<nrt::real> TurnRadiusParam("turnRadius",
      "The turning radius of Hermes.", 0.1143, ParamCat);
}

class HermesModule : public nrt::Module,
  public nrt::MessageSubscriber<hermes::VelocityCommand>,
  public nrt::MessagePoster<hermes::OdometryUpdatePort, hermes::Battery, hermes::CompassZ, hermes::GyroZ>
{
  public:
    HermesModule(std::string const& instanceName="");
    ~HermesModule();

    void run();

    void serialDevCallback(std::string const & dev);

    void onMessage(hermes::VelocityCommand msg);

    ResponsePacket writePacket(CommandPacket packet);

  private:
    std::vector<nrt::byte> serialRead(int ms, size_t length);

    std::mutex itsSerialPortMtx;
    std::mutex itsVelocityCommandMtx;
    std::vector<nrt::byte> itsMessageBuffer;
    CommandPacket itsVelocityCommand;
    boost::asio::io_service itsIO;
    boost::asio::serial_port itsSerialPort;
    nrt::Parameter<std::string> itsOdometryBaseFrame;
    nrt::Parameter<std::string> itsOdometryOdomFrame;
    nrt::Parameter<std::string> itsSerialDev;
    nrt::Parameter<nrt::real> itsTrimConstantParam;
    nrt::Parameter<nrt::real> itsBatteryCutoffParam;
    nrt::Parameter<nrt::real> itsForwardMaxParam;
    nrt::Parameter<nrt::real> itsBackwardMaxParam;
    nrt::Parameter<nrt::real> itsTurnRadiusParam;
    nrt::Time itsLastCommandTime;
    float itsLastBatteryReading;
};

#endif // HERMES_H 
