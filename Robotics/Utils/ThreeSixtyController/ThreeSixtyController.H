#ifndef SIXAXIS_H 
#define SIXAXIS_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>
#include <nrt/Robotics/Messages/JoystickMessage.H>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

namespace nrt { class DisplayImageSink; }

namespace sixaxis 
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT( JoystickInfo,     nrt::JoystickMessage, void, "The joystick data");
  NRT_DECLARE_MESSAGEPOSTER_PORT(     VelocityCommand,  nrt::VelocityMessage, void, "A velocity to send");
}

//! A simple module to allow joystick control of robots with a Sony ThreeSixtyController controller
/*! This module simply opens a new GTK window, and allows users to use a ThreeSixtyController controller to send out
  velocity command messages. */
class ThreeSixtyControllerModule : public nrt::Module,
                                   public nrt::MessageSubscriber<sixaxis::JoystickInfo>,
                                   public nrt::MessagePoster<sixaxis::VelocityCommand>
{
  public:
    ThreeSixtyControllerModule(std::string const& instanceName="");

    void run(); 

    virtual void onMessage(sixaxis::JoystickInfo msg);

  private:
    std::shared_ptr<nrt::DisplayImageSink> itsDisplaySink;
    std::mutex itsMtx;
    double itsAngularVel;
    double itsLinearVel;
};

#endif // SIXAXIS_H 
