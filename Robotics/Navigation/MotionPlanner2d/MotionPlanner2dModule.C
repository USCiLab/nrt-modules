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
  itsAngularPidComponent(new PIDComponent("angular")),
  itsTranslationalPidComponent(new PIDComponent("translation"))
{
  addSubComponent(itsAngularPidComponent);
  addSubComponent(itsTranslationalPidComponent);
}

// ######################################################################
double MotionPlanner2dModule::RotateToFaceTarget(Eigen::Translation3d const & translation)
{
  double angle = atan2(translation.y(), translation.x());

  itsAngularPidComponent->setObservedValue(-angle);
  itsAngularPidComponent->setDesiredValue(0.0);
  return itsAngularPidComponent->update();
}
// ######################################################################
double MotionPlanner2dModule::TranslateToReachTarget(Eigen::Translation3d const & translation)
{
  itsTranslationalPidComponent->setObservedValue(-translation.x());
  itsTranslationalPidComponent->setDesiredValue(0.0);
  return itsTranslationalPidComponent->update();
}

// ######################################################################
double MotionPlanner2dModule::RotateToTarget(nrt::Transform3D const & transform)
{
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
     *    translateToReachTarget(
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

    VelocityMessage::unique_ptr msg(new VelocityMessage);
    msg->linear.x() = 0.0;
    msg->linear.y() = 0.0;
    msg->linear.z() = 0.0;
    msg->angular.x() = 0.0;
    msg->angular.y() = 0.0;
    msg->angular.z() = 0.0;

    if (translation.x() > itsDistanceThresholdParam.getVal() && fabs(GetAngle(translation)) > itsRotationThresholdParam.getVal())
      msg->angular.z() = RotateToFaceTarget(translation);
    else if (translation.x() > itsDistanceThresholdParam.getVal())
    {
      msg->linear.x() = TranslateToReachTarget(translation);
      msg->angular.z() = RotateToFaceTarget(translation);
    }
    else
      msg->angular.z() = RotateToTarget(transform);

    NRT_INFO("Posting velocity message with angular component " << msg->angular.z() << " and linear component " << msg->linear.x());
    post<VelocityCommand>(msg);
  }
}

NRT_REGISTER_MODULE(MotionPlanner2dModule);
