#include "Joystick.H"
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/Text.H>
#include <nrt/Core/Util/Time.H>

using namespace nrt;
using namespace joystick;

// ######################################################################
JoystickModule::JoystickModule(std::string const & instanceName) : 
  Module(instanceName),
  itsUpdateRate(UpdateRateParam, this),
  itsDisplaySink(new DisplayImageSink),
  axes({}),
  buttons({}),
  itsJoystickOpen(false),
  itsJoystickDev(JoystickDevParam, this, &JoystickModule::joystickDevCallback)
{
  addSubComponent(itsDisplaySink);
}

// ######################################################################
void JoystickModule::joystickDevCallback(std::string const & dev)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsJoystickOpen = false;

  char joy_name[80];

  if (dev == "")
    return;

  // open the joystick device
  if ((joy_fd = open(dev.c_str(), O_RDONLY)) == -1)
  {
    throw exception::BadParameter(std::string("Open failed")); 
  }

  // get the number of axes and buttons from the joystick driver
  int num_axes;
  ioctl(joy_fd, JSIOCGAXES, &num_axes);
  int num_buttons;
  ioctl(joy_fd, JSIOCGBUTTONS, &num_buttons);
  ioctl(joy_fd, JSIOCGNAME(80), &joy_name);

  axes.resize(num_axes);
  buttons.resize(num_buttons);

  NRT_INFO("Detected joystick: " << joy_name << " " << num_axes << " axes and " << num_buttons << " buttons.");

  // operate the joystick driver in non-blocking mode
  fcntl(joy_fd, F_SETFL, O_NONBLOCK);
  itsJoystickOpen = true;
}
!s!@
// ######################################################################
void JoystickModule::run()
{
  NRT_INFO("Running : " << axes.size() << " [" << num_axes << "] , " << buttons.size() << " [" << num_buttons << "]");

  while(running())
  {
    std::lock_guard<std::mutex> _(itsMtx);

    auto waitTime = std::chrono::milliseconds(int(1000 / itsUpdateRate.getVal()));
    auto endTime = nrt::now() + waitTime;

    if(!itsJoystickOpen)
    {
      usleep(std::chrono::microseconds(waitTime).count());
      continue;
    }
    
    while(nrt::now() < endTime)
    {
      usleep(1000);
      js_event js;
      int n = read(joy_fd, &js, sizeof(struct js_event));
      if(n <= 0)
      {
        if(n == -1 && errno != EAGAIN) NRT_WARNING("Error reading from joystick");
        continue;
      }

      switch (js.type & ~JS_EVENT_INIT)
      {
        case JS_EVENT_AXIS:
          if(js.number >= axes.size()) { NRT_WARNING("Out of bounds axes: " << js.number << " / " << axes.size()); continue; }
          axes[js.number] = js.value;
          break;

        case JS_EVENT_BUTTON:
          if(js.number >= buttons.size()) { NRT_WARNING("Out of bounds axes: " << js.number << " / " << buttons.size()); continue; }
          buttons[js.number] = js.value;
          break;

        default:
          NRT_INFO("Unexpected result from joystick device");
          break;
      }
    }
    y

    Image<PixRGB<byte>> image(640, 480, ImageInitPolicy::Zeros);
    for (int i = 0; i < axes.size(); i++)
      nrt::drawText(image, Point2D<int32>(10, 20*i), nrt::sformat("axis %d: %d", i, axes[i])); 
    itsDisplaySink->out(GenericImage(image), "Velocity Commander");

    JoystickMessage::unique_ptr msg(new JoystickMessage);
    msg->axes    = axes;
    msg->buttons = buttons;

    post<JoystickCommand>(msg);
  }
}

NRT_REGISTER_MODULE(JoystickModule);
