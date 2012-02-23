#ifndef THREESIXTY_H 
#define THREESIXTY_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>
#include <Messages/Controllers/JoystickMessage.H>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

namespace nrt { class DisplayImageSink; }

namespace threesixty 
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT( JoystickInfo, nrt::JoystickMessage, void, "The joystick data");
  NRT_DECLARE_MESSAGEPOSTER_PORT(VelocityCommand, nrt::VelocityMessage, void, "A velocity to send");

  static const nrt::ParameterCategory ParamCat("Xbox360 Controller related options");

  static const nrt::ParameterDef<double> LinearMaxParam("linearmax", "The maximum linear velocity (m/s)", 10.0, ParamCat);
  static const nrt::ParameterDef<double> AngularMaxParam("angularmax", "The maximum angular velocity (rad/s)", 10.0, ParamCat);
}

//! A simple module to allow joystick control of robots with a Sony ThreeSixtyController controller
/*! This module simply opens a new GTK window, and allows users to use a ThreeSixtyController controller to send out
  velocity command messages. */
class ThreeSixtyControllerModule : public nrt::Module,
                                   public nrt::MessageSubscriber<threesixty::JoystickInfo>,
                                   public nrt::MessagePoster<threesixty::VelocityCommand>
{
  public:
    ThreeSixtyControllerModule(std::string const& instanceName="");

    void run(); 

    virtual void onMessage(threesixty::JoystickInfo msg);

    void linearMaxChangeCallback(double const & newmax);

    void angularMaxChangeCallback(double const & newmax);

    double deadZoneAdjusted(double n, double min, double center, double max);

  private:
    std::shared_ptr<nrt::DisplayImageSink> itsDisplaySink;
    std::mutex itsMtx;
    double itsAngularVel;
    double itsLinearVel;
    nrt::Parameter<double> itsLinearMax;
    nrt::Parameter<double> itsAngularMax;
    double itsLinearScaling;
    double itsAngularScaling;
};

#endif // THREESIXTY_H 
