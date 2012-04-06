#include "ThreeSixtyController.H"
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/Text.H>

using namespace nrt;
using namespace threesixty;

/*
 * Xbox 360 controller has 8 axes
 * axis0+ is move right on left joystick
 * axis0- is move left on left joystick
 * axis1+ is move down on left joystick
 * axis1- is move up on left joystick
 * axis2+ is left trigger fully down
 * axis2- is left trigger fully up
 * axis3+ is move right on right joystick
 * axis3- is move left on right joystick
 * axis4+ is move down on right joystick
 * axis4- is move up on right joystick
 * axis5+ is right trigger fully down
 * axis5- is right trigger fully up
 * axis6+ is right on dpad
 * axis6- is left on dpad
 * axis7+ is down on dpad
 * axis7- is up on dpad
 *
 * button values are 1
 * button0: a
 * button1: b
 * button2: x
 * button3: y
 * button4: lb
 * button5: rb
 * button6: select
 * button7: start
 * button8: center button
 * button9: left joystick down
 * button10: right joystick down
 */

// ######################################################################
ThreeSixtyControllerModule::ThreeSixtyControllerModule(std::string const & instanceName) :
  Module(instanceName),
  itsDisplaySink(new DisplayImageSink),
  itsLinearMax(LinearMaxParam, this, &ThreeSixtyControllerModule::linearMaxChangeCallback),
  itsAngularMax(AngularMaxParam, this, &ThreeSixtyControllerModule::angularMaxChangeCallback)
{ 
  addSubComponent(itsDisplaySink);
}

// ######################################################################
void ThreeSixtyControllerModule::linearMaxChangeCallback(double const & newmax)
{
  itsLinearScaling = -32767.0/newmax;
}

// ######################################################################
void ThreeSixtyControllerModule::angularMaxChangeCallback(double const & newmax)
{
  itsAngularScaling = 32767.0/newmax;
}

// ######################################################################
double ThreeSixtyControllerModule::deadZoneAdjusted(double n, double min, double center, double max)
{
  if (fabs(n-center) < 0.05*(max-min))
    return center;
  return n;
}

// ######################################################################
void ThreeSixtyControllerModule::onMessage(threesixty::JoystickInfo msg)
{
  std::lock_guard<std::mutex> lock(itsMtx);

  // axes 0, 1, 3, 4 are joysticks which vary from -32767 to 32767 (centered at 0)
  // let the dead zone be 10% of the range

  itsLinearVel  = deadZoneAdjusted(msg->axes[1], -32767, 0, 32767) / itsLinearScaling;
  itsAngularVel = deadZoneAdjusted(msg->axes[3], -32767, 0, 32767) / itsAngularScaling;
}

// ######################################################################
void ThreeSixtyControllerModule::run()
{
  double linear = 0.0;
  double angular = 0.0;
  while(running())
  {
    usleep(10000);
    Image<PixRGB<byte>> image(300, 60, ImageInitPolicy::Zeros);
    nrt::drawText(image, Point2D<int32>(10, 10), nrt::sformat("Translational Vel: %f", itsLinearVel));
    nrt::drawText(image, Point2D<int32>(10, 30), nrt::sformat("Rotational Vel   : %f", itsAngularVel));
    itsDisplaySink->out(GenericImage(image), "Xbox 360 velocities");

    if ( !(itsLinearVel == linear && itsAngularVel == angular) )
    {
      VelocityMessage::unique_ptr msg(new VelocityMessage);
      msg->linear.x()  = itsLinearVel;
      msg->angular.z() = itsAngularVel;
      post<VelocityCommand>(msg);
      
      linear = itsLinearVel;
      angular = itsAngularVel;
    }
  }
}

NRT_REGISTER_MODULE(ThreeSixtyControllerModule);
