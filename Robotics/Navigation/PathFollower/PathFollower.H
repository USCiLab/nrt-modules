#ifndef NRT_MODULES_ROBOTICS_NAVIGATION_PATHFOLLOWER_H
#define NRT_MODULES_ROBOTICS_NAVIGATION_PATHFOLLOWER_H

namespace pathfollower
{
  // takes in a path
  // checks a transform
  // outputs a velocity command
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(Path, nrt::PathMessage, "The path to follow.");
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformLookup, nrt::TransformLookup, nrt::TransformMessage, "The transform lookup port.");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Velocity, nrt::VelocityMessage, void, "The velocity that will get to this path.");

  static const nrt::ParameterCategory ParamCat("PathFollower related options.");

  static const nrt::ParameterDef<std::string> FromTransformParam("from", "The name of the base transform.", "world");

  static const nrt::ParameterDef<std::string> ToTransformParam("to", "The name of the transform whose velocity will be commanded.", "robot");

  static const nrt::ParameterDef<bool> ShowDebugParam("showDebugMap", "Show the debug map.", false);
}

class PathFollowerModule : public nrt::Module,
  public nrt::MessageSubscriber<pathfollower::Path>,
  public nrt::MessagePoster<pathfollower::TransformLookup, pathfollower::Velocity>
{
  public:
    PathFollowerModule(std::string const & instanceName="");

    nrt::Transform3D lookupTransform(std::string from, std::string to);

    void onMessage(pathfollower::Path msg);

  private:
    void debugParamCallback(bool const & debug);

    std::mutex itsMtx;
    std::shared_ptr<nrt::DisplayImageSink> itsDisplay;
    nrt::Parameter<std::string> itsFromTransformParam;
    nrt::Parameter<std::string> itsToTransformParam;
    nrt::Parameter<bool> itsShowDebugParam;
};

#endif //NRT_MODULES_ROBOTICS_NAVIGATION_PATHFOLLOWER_H
