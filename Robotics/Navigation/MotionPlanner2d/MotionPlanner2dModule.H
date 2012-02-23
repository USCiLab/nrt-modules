#ifndef MOTIONPLANNER2DMODULE_H
#define MOTIONPLANNER2DMODULE_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>
#include <nrt/Robotics/Components/PIDComponent.H>

namespace motionplanner2d
{
  NRT_DECLARE_TRANSFORM_LOOKUP_PORT( TransformLookupPort );
  NRT_DECLARE_MESSAGEPOSTER_PORT( VelocityCommand, nrt::VelocityMessage, void, "The velocity to send" );

  static const nrt::ParameterCategory ParamCat("Motion Planner related options");

  static const nrt::ParameterDef<std::string> FromFrameParam("from", "The origin of the plan (a named Transform in the TransformManager)", "robot", ParamCat);

  static const nrt::ParameterDef<std::string> TargetFrameParam("target", "The target of the plan (a named Transform in the TransformManager)", "target", ParamCat);

  static const nrt::ParameterDef<double> DistanceThresholdParam("distance-threshold", "The distance to the target that MotionPlanner will try to get within", 0.1, ParamCat);

  static const nrt::ParameterDef<double> RotationThresholdParam("rotation-threshold", "The angle to the target that MotionPlanner will try to get within", 0.1, ParamCat);

  static const nrt::ParameterDef<std::string> OutputFrameParamDef("output-frame", 
      "The name of the dead-reckoned robot frame", "motionplanner2d", ParamCat);
}

class MotionPlanner2dModule : public nrt::Module,
  public nrt::MessagePoster<motionplanner2d::TransformLookupPort, motionplanner2d::VelocityCommand>
{
  public:
    MotionPlanner2dModule(std::string const& instanceName="");

    void run();

  private:
     double RotateToFaceTarget(Eigen::Translation<double, 3> const & translation);
    
     double TranslateToReachTarget(Eigen::Translation<double, 3> const & translation);

     double RotateToTarget(nrt::Transform3D const & transform);

    double GetAngle(Eigen::Translation3d const & translation);

    nrt::Parameter<std::string> itsFromFrameParam;
    nrt::Parameter<std::string> itsTargetFrameParam;
    nrt::Parameter<double> itsDistanceThresholdParam;
    nrt::Parameter<double> itsRotationThresholdParam;
    std::shared_ptr<nrt::PIDComponent> itsAngularPidComponent;
    std::shared_ptr<nrt::PIDComponent> itsTranslationalPidComponent;
};

#endif // MOTIONPLANNER2DMODULE_H
