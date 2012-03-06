#include "DeadReckoningModule.H"
#include <SerialPort.h>
#include <nrt/Core/Util/MathUtils.H>

using namespace nrt;
using namespace deadreckoning; 

// ######################################################################
DeadReckoningModule::DeadReckoningModule(std::string const& instanceName) :
  Module(instanceName),
  itsBaseFrameParam(BaseFrameParamDef, this),
  itsDeadReckoningFrameParam(OutputFrameParamDef, this),
  itsTransform(Eigen::Translation3d::Identity())
{ }

// ######################################################################
void DeadReckoningModule::extrapolatePosition()
{
  // Get the time since last update in seconds
  double deltaTime = nrt::Duration(nrt::now() - itsLastUpdateTime).count();

  if(itsLastVelocity.angular.z() == 0)
  {
    itsTransform = itsTransform * Eigen::Translation3d(itsLastVelocity.linear.x() * deltaTime, 0, 0);
  }
  else if(itsLastVelocity.linear.x() == 0)
  {
    itsTransform = itsTransform * Eigen::AngleAxisd(itsLastVelocity.angular.z() * deltaTime, Eigen::Vector3d::UnitZ());
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
    double chordLength = sin(dtheta/2.0) * radius * 2.0;

    // The angle made between the radius, and the chord
    double theta2 = NRT_D_PI/2.0 - dtheta/2.0;

    // The actual displacement from our current position
    Eigen::Translation3d translation(
        sin(theta2) * chordLength,
        cos(theta2) * chordLength,
        1);

    itsTransform = itsTransform * nrt::Transform3D(translation);  
    itsTransform = itsTransform * Eigen::AngleAxisd(dtheta, Eigen::Vector3d::UnitZ());
  }

  std::unique_ptr<TransformMessage> outmsg(new TransformMessage(
        nrt::now(), itsBaseFrameParam.getVal(), itsDeadReckoningFrameParam.getVal(), itsTransform));
  post<DeadReckoningOutput>(outmsg);
  itsLastUpdateTime = nrt::now();
}

// ######################################################################
void DeadReckoningModule::onMessage(VelocityCommand msg)
{
  std::lock_guard<std::mutex> _(itsMtx);

  // Uh oh! We can't handle full screw movement right now, so let's throw an exception if
  // users are trying to mess around with other velocities
  if(msg->linear.y() != 0 || msg->linear.z() != 0 || msg->angular.x() != 0 || msg->angular.y() != 0)
    throw NRT_MODULE_EXCEPTION("Only linear.x and angular.z velocities are currently supported"); 

  if(itsLastUpdateTime != nrt::Time()) extrapolatePosition();

  itsLastVelocity   = *msg;
}

// ######################################################################
void DeadReckoningModule::run()
{
  {
    std::lock_guard<std::mutex> _(itsMtx);
    // Reset our transform
    itsTransform = Eigen::Translation3d::Identity();
  }

  while(running())
  {
    {
      std::lock_guard<std::mutex> _(itsMtx);
      if( nrt::Duration(nrt::now() - itsLastUpdateTime).count() > 0.1)
        extrapolatePosition();
    }
    usleep(100000);
  }
}

NRT_REGISTER_MODULE(DeadReckoningModule);



