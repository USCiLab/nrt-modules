#include "ThreeSixtyController.H"
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/Text.H>

using namespace nrt;
using namespace sixaxis;

// ######################################################################
ThreeSixtyControllerModule::ThreeSixtyControllerModule(std::string const & instanceName) :
  Module(instanceName),
  itsDisplaySink(new DisplayImageSink)
{ 
  addSubComponent(itsDisplaySink);
}

// ######################################################################
void ThreeSixtyControllerModule::onMessage(sixaxis::JoystickInfo msg)
{
  std::lock_guard<std::mutex> lock(itsMtx);
  itsLinearVel  = msg->axes[1] / 3276.0;
  itsAngularVel = msg->axes[0] / -1638.0;
}

// ######################################################################
void ThreeSixtyControllerModule::run()
{
  double linear = 0.0;
  double angular = 0.0;
  while(running())
  {
    Image<PixRGB<byte>> image(640, 480, ImageInitPolicy::Zeros);
    nrt::drawText(image, Point2D<int32>(10, 10), nrt::sformat("Translational Vel: %f", itsLinearVel));
    nrt::drawText(image, Point2D<int32>(10, 30), nrt::sformat("Rotational Vel   : %f", itsAngularVel));
    itsDisplaySink->out(GenericImage(image), "Velocity Commander");

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
