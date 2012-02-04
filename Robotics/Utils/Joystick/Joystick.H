#ifndef JOYSTICK_H 
#define JOYSTICK_H 

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/JoystickMessage.H>

namespace joystick
{
  NRT_DECLARE_MESSAGEPOSTER_PORT( JoystickCommand, nrt::JoystickMessage, void, "Joystick data" );

  static const nrt::ParameterCategory ParamCat("Joystick related options");

  static const nrt::ParameterDef<std::string> JoystickDevParam("joystickdev", "The joystick device file", "", ParamCat);
}

//! A simple module that exposes linux/joystick.h as an NRT module */
class JoystickModule : public nrt::Module, public nrt::MessagePoster<joystick::JoystickCommand>
{
  public:
    JoystickModule(std::string const& instanceName="");

    void run();

    void joystickDevCallback(std::string const& dev);

  private:
    std::mutex itsMtx;
    nrt::Parameter<std::string> itsJoystickDev;
    int joy_fd, num_axes, num_buttons;
    std::vector<int> axis;
    std::vector<int> button;
    struct js_event js;
};

#endif // JOYSTICK_H 
