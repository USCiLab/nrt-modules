#include "Joystick.H"

using namespace nrt;
using namespace joystick;

// ######################################################################
JoystickModule::JoystickModule(std::string const & instanceName) : 
  Module(instanceName),
  itsJoystickDev(JoystickDevParam, this, &JoystickModule::joystickDevCallback)
{
  // Empty
}

// ######################################################################
void JoystickModule::joystickDevCallback(std::string const & dev)
{
  std::lock_guard<std::mutex> lock(itsMtx);

  if (dev == "")
    return;

  // open the joystick device
  if ((joy_fd = open(dev.c_str(), O_RDONLY)) == -1)
  {
    throw exception::BadParameter(std::string("Open failed")); 
  }

  // get the number of axes and buttons from the joystick driver
  ioctl(joy_fd, JSIOCGAXES, &num_axes);
  ioctl(joy_fd, JSIOCGBUTTONS, &num_buttons);

  // operate the joystick driver in non-blocking mode
  fcntl(joy_fd, F_SETFL, O_NONBLOCK);
}

// ######################################################################
void JoystickModule::run()
{
  while(running())
  {
    JoystickMessage::unique_ptr msg(new JoystickMessage);
    msg->axes.reserve(num_axes);
    msg->buttons.reserve(num_buttons);
    {
      std::lock_guard<std::mutex> lock(itsMtx);

      read(joy_fd, &js, sizeof(struct js_event));

      switch (js.type & ~JS_EVENT_INIT)
      {
        case JS_EVENT_AXIS:
          msg->axes[js.number] = js.value;
          break;

        case JS_EVENT_BUTTON:
          msg->buttons[js.number] = js.value;
          break;
      }
    }
    post<JoystickCommand>(msg);
  }
}

NRT_REGISTER_MODULE(JoystickModule);
