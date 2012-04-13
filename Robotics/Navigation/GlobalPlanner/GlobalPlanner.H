#ifndef NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H
#define NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <Messages/ImageProc/GenericImageMessage.H>

namespace globalplanner
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(OccupancyMap, GenericImageMessage, void, "The occupancy map represented as a grayscale image.");
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformLookup, nrt::TransformLookupMessage, nrt::TransformMessage, "The transform lookup port");
  NRT_DECLARE_MESSAGEPOSTER_PORT(NextTransform, nrt::TransformMessage, void, "The next position the robot should go to.");

  static const nrt::ParameterCategory ParamCat("GlobalPlanner related options");

  static const nrt::ParameterDef<std::string> NextTransformParam("transform", "The name of the transform to update.", "carrot");

  static const nrt::ParameterDef<std::string> WorldTransformParam("world", "The name of the world transform.", "world");

  static const nrt::ParameterDef<std::string> RobotTransformParam("robot", "The name of the robot's transform.", "robot");

  static const nrt::ParameterDef<std::string> GoalTransformParam("goal", "The name of the transform we are trying to reach.", "goal");

  static const nrt::ParameterDef<float> IntervalParam("interval", "The number of meters after which we re-calculate a new path", 0.5);
}

class GlobalPlannerModule : public nrt::Module,
  public nrt::MessageSubscriber<globalplanner::OccupancyMap>,
  public nrt::MessagePoster<globalplanner::TransformLookup, globalplanner::NextTransform>
{
  public:
    GlobalPlannerModule(std::string const& instanceName="");

    void onMessage(globalplanner::OccupancyMap map);

    nrt::Transform3D lookupTransform(std::string from, std::string to);

  private:
    std::mutex itsMtx;
    float itsPixelsPerMeter;
    nrt::Parameter<std::string> itsNextTransformParam;
    nrt::Parameter<std::string> itsWorldTransformParam;
    nrt::Parameter<std::string> itsRobotTransformParam;
    nrt::Parameter<std::string> itsGoalTransformParam;
    nrt::Parameter<float> itsIntervalParam;
    nrt::Transform3D itsLastBreadcrumb;
};


#endif // NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H
