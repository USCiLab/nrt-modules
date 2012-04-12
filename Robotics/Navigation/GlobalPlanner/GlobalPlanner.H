#ifndef NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H
#define NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>

namespace globalplanner
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(OccupancyMap, GenericImage, void, "The occupancy map represented as a grayscale image.");
  NRT_DECLARE_MESSAGEPOSTER_PORT(NextTransform, nrt::TransformMessage, void, "The next position the robot should go to.");

  static const nrt::ParameterCategory ParamCat("GlobalPlanner related options");

  static const nrt::ParameterDef<std::string> NextTransformParam("transform", "The name of the transform to update.", "robot");
}

class GlobalPlannerModule : public nrt::Module,
  public nrt::MessageSubscriber<globalplanner::OccupancyMap>,
  public nrt::MessagePoster<globalplanner::NextTransform>
{
  public:
    GlobalPlannerModule(std::string const& instanceName="");

    void run();

    void onMessage(globalplanner::OccupancyMap map);

  private:
    nrt::Parameter<std::string> itsNextTransformParam;
};


#endif // NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H
