#include "VelocityCommander.H"
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/Text.H>


using namespace nrt;
using namespace velocitycommander;

// ######################################################################
VelocityCommanderModule::VelocityCommanderModule(std::string const & instanceName) :
  Module(instanceName),
  itsDisplaySink(new DisplayImageSink),
  itsAngularVel(0),
  itsLinearVel(0)
{ 
  addSubComponent(itsDisplaySink);
  itsDisplaySink->setKeyCallback(std::bind(&VelocityCommanderModule::keyCallback, this, std::placeholders::_1));
}


// ######################################################################
void VelocityCommanderModule::keyCallback(nrt::int32 key)
{
  std::lock_guard<std::mutex> lock(itsMtx);
#define KEY_UP    65362
#define KEY_DOWN  65364
#define KEY_LEFT  65361
#define KEY_RIGHT 65363
#define KEY_SPACE 32
  switch(char(key))
  {
    case 'w':
      itsLinearVel += .01;
      break;
    case 'a':
      itsLinearVel -= .01;
      break;
    case 's':
      itsAngularVel -= .01;
      break;
    case 'd':
      itsAngularVel += .01;
      break;
    default:
      itsLinearVel = 0;
      itsAngularVel = 0;
      break;
  }
}

// ######################################################################
void VelocityCommanderModule::run()
{
  while(running())
  {
    double linear, angular; 
    {
      std::lock_guard<std::mutex> lock(itsMtx);
      linear = itsLinearVel;
      angular = itsAngularVel;
    }

    Image<PixRGB<byte>> image(640, 480, ImageInitPolicy::Zeros);
    nrt::drawText(image, Point2D<int32>(10, 10), nrt::sformat("Translational Vel: %f", linear));
    nrt::drawText(image, Point2D<int32>(10, 30), nrt::sformat("Rotational Vel   : %f", angular));
    itsDisplaySink->out(GenericImage(image), "Velocity Commander");

    VelocityMessage::unique_ptr msg(new VelocityMessage);
    msg->linear.x()  = linear;
    msg->linear.y()  = 0;
    msg->linear.z()  = 0;
    msg->angular.x() = 0;
    msg->angular.y() = 0;
    msg->angular.z() = angular;
    try { post<VelocityCommand>(msg); }
    catch(...) {}

    usleep(100000);
  }
}

NRT_REGISTER_MODULE(VelocityCommanderModule);
