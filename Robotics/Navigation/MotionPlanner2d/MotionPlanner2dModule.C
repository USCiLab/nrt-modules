#include "MotionPlanner2dModule.H"

using namespace nrt;
using namespace motionplanner2d; 

// ######################################################################
MotionPlanner2dModule::MotionPlanner2dModule(std::string const& instanceName) :
  Module(instanceName),
  itsFromFrameParam(FromFrameParam, this),
  itsTargetFrameParam(TargetFrameParam, this),
  itsDistanceThresholdParam(DistanceThresholdParam, this),
  itsRotationThresholdParam(RotationThresholdParam, this),
  itsAngularPidComponent(new PIDComponent),
  itsTranslationalPidComponent(new PIDComponent)
{
  NRT_INFO(__LINE__); 
  addSubComponent(itsAngularPidComponent);
  addSubComponent(itsTranslationalPidComponent);
}

// ######################################################################
nrt::VelocityMessage MotionPlanner2dModule::RotateToFaceTarget(Eigen::Translation3d const & translation)
{
  // get the translation component from transform
  // find the angle to the translation component
  // construct a VelocityMessage
  // set the message->angular to minimize the angle
  // return the message
  double angle = atan2(translation.y(), translation.x());

  itsAngularPidComponent->setObservedValue(angle);
  itsAngularPidComponent->setDesiredValue(0.0);
  double result = itsAngularPidComponent->update();
  
  VelocityMessage velocity;
  velocity.angular.z() = result;
  return velocity;
}
// ######################################################################
nrt::VelocityMessage MotionPlanner2dModule::TranslateToReachTarget(Eigen::Translation3d const & translation)
{
  // get the translation component from transform
  // construct a VelocityMessage
  // set the message->linear to minimize the distance
  // return the message
  itsTranslationalPidComponent->setObservedValue(translation.x());
  itsTranslationalPidComponent->setDesiredValue(0.0);
  double result = itsTranslationalPidComponent->update();

  VelocityMessage velocity;
  velocity.linear.x() = result;
  return velocity;
}

// ######################################################################
nrt::VelocityMessage MotionPlanner2dModule::RotateToTarget(nrt::Transform3D const & transform)
{
  // get the translation component from transform
  // construct a new Transform3d object displaced by 1 from transform along the x-axis
  // return RotateToFaceTarget(this new transform)
  nrt::Transform3D displaced(transform);
  displaced.translate(Eigen::Vector3d(1.0, 0.0, 0.0));

  return RotateToFaceTarget((Eigen::Translation3d)displaced.translation());
}

double MotionPlanner2dModule::GetAngle(Eigen::Translation3d const & translation)
{
  return atan2(translation.y(), translation.x());
}

// ######################################################################
void MotionPlanner2dModule::run()
{
  while (running())
  {
    /*
     * T = get the transform from robot to target
     * def rotateToFaceTarget:
     *    t = get the translation component from T
     *    find the angle between my X component and t
     *    minimize that angle
     * def translateToReachTarget:
     *    t = get the translation component from T
     *    minimize that component
     * def rotateToTarget:
     *    r = get the rotation component from T
     *    minimize that component
     *
     * if T is farther away than some threshold and T.rotation is not close to zero:
     *    rotateToFaceTarget()
     * else if T is farther away than some threshold:
     *    translateToReachTarget()
     * else:
     *    rotateToTarget()
     */
    
    TransformLookupMessage::unique_ptr transformLookupMsg(new TransformLookupMessage( nrt::now(), itsFromFrameParam.getVal(), itsTargetFrameParam.getVal() ));
    nrt::MessagePosterResults<TransformLookupPort> results = post<TransformLookupPort>(transformLookupMsg);
    if(results.size() == 0)
    {
      NRT_WARNING("No Transform Manager Connected!");
      return;
    }
    std::shared_ptr<nrt::TransformMessage const> transformMessage = results.get();

    nrt::Transform3D transform = transformMessage->transform;
    Eigen::Translation3d translation = (Eigen::Translation3d)transform.translation();
    std::unique_ptr<VelocityMessage> msg;

    if (translation.x() > itsDistanceThresholdParam.getVal() && abs(GetAngle(translation)) > itsRotationThresholdParam.getVal())
      msg.reset(new VelocityMessage(RotateToFaceTarget(translation)));
    else if (translation.x() > itsDistanceThresholdParam.getVal())
      msg.reset(new VelocityMessage(TranslateToReachTarget(translation)));
    else
      msg.reset(new VelocityMessage(RotateToTarget(transform)));

    post<VelocityCommand>(msg);
  }
}

NRT_REGISTER_MODULE(MotionPlanner2dModule);
