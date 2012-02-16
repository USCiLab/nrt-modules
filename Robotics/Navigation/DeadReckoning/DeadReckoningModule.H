#ifndef DEADRECKONINGMODULE_H
#define DEADRECKONINGMODULE_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>

namespace deadreckoning
{
  NRT_DECLARE_TRANSFORM_UPDATE_PORT( DeadReckoningOutput );
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT( VelocityCommand, nrt::VelocityMessage, void, "The requested velocity of the robot" );

  static const nrt::ParameterCategory ParamCat("Dead Reckoning Related Options");

  static const nrt::ParameterDef<std::string> BaseFrameParamDef("base-frame", 
      "The name of the base frame from which the output dead reckoning frame is referenced. "
      " This is usually just the 'world' frame", "world", ParamCat);

  static const nrt::ParameterDef<std::string> OutputFrameParamDef("output-frame", 
      "The name of the dead-reckoned robot frame", "deadreckoning", ParamCat);
}

class DeadReckoningModule : public nrt::Module,
  public nrt::MessageSubscriber<deadreckoning::VelocityCommand>,
  public nrt::MessagePoster<deadreckoning::DeadReckoningOutput>
{
  public:
    DeadReckoningModule(std::string const& instanceName="");

    void onMessage(deadreckoning::VelocityCommand msg);

    void run();

  private:
    std::mutex itsMtx;
    nrt::Parameter<std::string> itsBaseFrameParam;
    nrt::Parameter<std::string> itsDeadReckoningFrameParam;
    nrt::Time itsLastUpdateTime;
    nrt::VelocityMessage itsLastVelocity;
    nrt::Transform3D itsTransform;
};

#endif // DEADRECKONINGMODULE_H
