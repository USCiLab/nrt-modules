#include "SixAxis.H"
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/Text.H>

using namespace nrt;
using namespace sixaxis;

// ######################################################################
SixAxisModule::SixAxisModule(std::string const & instanceName) :
	Module(instanceName),
	itsDisplaySink(new DisplayImageSink),
  itsJoystickDev(JoystickDevParam, this, &SixAxisModule::joystickDevCallback)
{ 
	addSubComponent(itsDisplaySink);
}

// ######################################################################
void SixAxisModule::joystickDevCallback(std::string const & dev)
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
  
  axis.reserve(num_axes);
  button.reserve(num_buttons);

  // operate the joystick driver in non-blocking mode
  fcntl(joy_fd, F_SETFL, O_NONBLOCK);
}

// ######################################################################
void SixAxisModule::run()
{
	while(running())
	{
	  double linear, angular; 
		{
		  std::lock_guard<std::mutex> lock(itsMtx);
     
      read(joy_fd, &js, sizeof(struct js_event));
      
      switch (js.type & ~JS_EVENT_INIT)
      {
        case JS_EVENT_AXIS:
          axis[js.number] = js.value;
          break;

        case JS_EVENT_BUTTON:
          button[js.number] = js.value;
          break;
      }

      // for the xbox 360 controller, we care about axis[0] and axis[1] (the X and Y axes)
      // these correspond to the left joystick moving up and down
		  // all the way up =>    Y = -32767
      // all the way down =>  Y = +32767
      // all the way left =>  X = -32767
      // all the way right => X = +32767
    
      linear = axis[1] / 3276.0;
      angular = axis[0] / -1638.0;
		}

		Image<PixRGB<byte>> image(640, 480, ImageInitPolicy::Zeros);
		nrt::drawText(image, Point2D<int32>(10, 10), nrt::sformat("Translational Vel: %f (raw: %d)", linear, axis[1]));
		nrt::drawText(image, Point2D<int32>(10, 30), nrt::sformat("Rotational Vel   : %f (raw: %d)", angular, axis[0]));
		itsDisplaySink->out(GenericImage(image), "Velocity Commander");

    if ( !(itsLinearVel == linear && itsAngularVel == angular) )
    {
      VelocityMessage::unique_ptr msg(new VelocityMessage);
      msg->linear.x()  = linear;
      msg->angular.z() = angular;
      post<VelocityCommand>(msg);
      itsLinearVel = linear;
      itsAngularVel = angular;
    }
		//usleep(100000);
	}
}

NRT_REGISTER_MODULE(SixAxisModule);
