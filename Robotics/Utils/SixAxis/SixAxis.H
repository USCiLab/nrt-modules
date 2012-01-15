#ifndef SIXAXIS_H 
#define SIXAXIS_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

namespace nrt { class DisplayImageSink; }

namespace sixaxis 
{
  NRT_DECLARE_MESSAGEPOSTER_PORT( VelocityCommand, nrt::VelocityMessage, void, "A velocity to send" );

  static const nrt::ParameterCategory ParamCat("SixAxis Related Options");

  static const nrt::ParameterDef<std::string> JoystickDevParam("joystickdev",
      "The joystick device file", "", ParamCat);
}

//! A simple module to allow joystick control of robots with a Sony SixAxis controller
/*! This module simply opens a new GTK window, and allows users to use a SixAxis controller to send out
    velocity command messages. */
class SixAxisModule : public nrt::Module,
                      public nrt::MessagePoster<sixaxis::VelocityCommand>
{
  public:
    SixAxisModule(std::string const& instanceName="");

    void run();

    void joystickDevCallback(std::string const & dev);

  private:
	std::mutex itsMtx;
	std::shared_ptr<nrt::DisplayImageSink> itsDisplaySink;
  nrt::Parameter<std::string> itsJoystickDev;
	double itsAngularVel;
	double itsLinearVel;
  int joy_fd, num_axes, num_buttons;
  std::vector<int> axis;
  std::vector<int> button;
  struct js_event js;
};

#endif // SIXAXIS_H 
