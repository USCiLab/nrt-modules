#ifndef INRTJOYSTICK_H 
#define INRTJOYSTICK_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>

namespace nrt { class DisplayImageSink; }

namespace inrtjoystick 
{
  NRT_DECLARE_MESSAGEPOSTER_PORT( VelocityCommand, nrt::VelocityMessage, void, "A velocity to send" );

  static const nrt::ParameterCategory ParamCat("iNRTJoystick Related Options");

  static const nrt::ParameterDef<int> PortParam("port", "The port to listen on", 61557, ParamCat);

  static const nrt::ParameterDef<int> MaxLinearVel("maxlinear", "Maximum linear velocity in m/s", 1, ParamCat);

  static const nrt::ParameterDef<int> MaxAngularVel("maxangular", "Maximum angular velocity in rad/s", 1, ParamCat);

  static const nrt::ParameterDef<std::string> WebviewURLParam("webview", "A URL that the phone should display in its web view", "http://google.com/", ParamCat);
}

//! A simple module to allow joystick control of robots with the iNRTJoystick iOS app 
/*! This module simply opens a new GTK window, and allows users to use the iNRTJoystick controller to send out
  velocity command messages. */
class iNRTJoystickModule: public nrt::Module,
  public nrt::MessagePoster<inrtjoystick::VelocityCommand>
{
  public:
    iNRTJoystickModule(std::string const& instanceName="");

    ~iNRTJoystickModule();

    void PortChangeCallback(int const & port);

    void WebviewURLChangeCallback(std::string const & webviewurl);
    
    void run();

  private:
    std::mutex itsMtx;
    nrt::Parameter<int> itsPort;
    nrt::Parameter<int> itsMaxLinearVelParam;
    nrt::Parameter<int> itsMaxAngularVelParam;
    nrt::Parameter<std::string> itsWebviewURL;
    std::shared_ptr<nrt::DisplayImageSink> itsDisplaySink;
    int itsLinearVel;
    int itsAngularVel;
    int itsSocket;
    bool itsRunning;
};

#endif // INRTJOYSTICK_H 
