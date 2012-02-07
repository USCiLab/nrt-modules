#ifndef JOYSTICK_H 
#define JOYSTICK_H 

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/JoystickMessage.H>

namespace nrt { class DisplayImageSink; }

namespace joystick
{
  NRT_DECLARE_MESSAGEPOSTER_PORT( JoystickCommand, nrt::JoystickMessage, void, "Joystick data" );

  static const nrt::ParameterCategory ParamCat("Joystick related options");

  static const nrt::ParameterDef<std::string> JoystickDevParam("joystickdev", "The joystick device file", "/dev/input/js1", ParamCat);
  static const nrt::ParameterDef<float> UpdateRateParam("UpdateRate", "The update rate (in hz) to send messages", 10, ParamCat);
}

//! A simple module that exposes linux/joystick.h as an NRT module */
class JoystickModule : public nrt::Module, public nrt::MessagePoster<joystick::JoystickCommand>
{
  public:
    JoystickModule(std::string const& instanceName="");

    void run();

    void joystickDevCallback(std::string const& dev);

  private:
    bool itsJoystickOpen;
    std::shared_ptr<nrt::DisplayImageSink> itsDisplaySink;
    std::mutex itsMtx;
    int joy_fd, num_axes, num_buttons;
    std::vector<int> axes;
    std::vector<int> buttons;
    nrt::Parameter<std::string> itsJoystickDev;
    nrt::Parameter<float> itsUpdateRate;
};

#endif // JOYSTICK_H 
