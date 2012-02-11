#include "DeadReckoningModule.H"
#include <SerialPort.h>
#include <nrt/Core/Util/MathUtils.H>

using namespace nrt;
using namespace deadreckoning; 

// ######################################################################
DeadReckoningModule::DeadReckoningModule(std::string const& instanceName) :
  Module(instanceName),
  itsBaseFrameParam(BaseFrameParamDef, this),
  itsDeadReckoningFrameParam(OutputFrameParamDef, this)
{ }


// ######################################################################
void DeadReckoningModule::onMessage(VelocityCommand msg)
{
  std::lock_guard<std::mutex> _(itsMtx);

  if(msg->linear.y() != 0 || msg->linear.z() != 0 || msg->angular.x() != 0 || msg->angular.y() != 0)
    throw NRT_MODULE_EXCEPTION("The Dead Reckoning Module can only handle linear.x and angular.z velocities!");

  if(itsLastUpdateTime != nrt::Time())
  {
    // Get the time since last update in seconds
    double deltaTime = nrt::Duration(nrt::now() - itsLastUpdateTime).count();

    Eigen::Vector3d translation;
    Eigen::AngleAxisd rotation;
    if(msg->angular.z() == 0)
    {
    }
    else
    {
      // The amount we have turned (theta displacement) 
      double dtheta = itsLastVelocity.angular.z() * deltaTime;

      // The radius of the circle we have been following
      // This is calculated by solving: (angularVelocity * time)   (linearVelocity * time)  
      //                                ------------------------ = -----------------------
      //                                          (2*pi)                (2*pi*radius)
      double radius = itsLastVelocity.linear.x() / itsLastVelocity.angular.z();

      // The length of the chord from our last position to our current one
      double chordLength = (sin(dtheta) / radius) * 2;

      // The angle made between the radius, and the chord
      double oldTheta = 0; // <<<<<<<<<<< How do we get oldTheta??
      double theta2 = (NRT_D_PI / 2.0 - dtheta/2.0) + oldTheta;

      // The actual displacement from our current position
      translation =  Eigen::Vector3d(
          cos(theta2) * chordLength,
          sin(theta2) * chordLength,
          1);

      rotation = Eigen::AngleAxisd(dtheta, Eigen::Vector3d::UnitZ());
    }
  }


  itsLastVelocity   = *msg;
  itsLastUpdateTime = nrt::now();
}

NRT_REGISTER_MODULE(DeadReckoningModule);



