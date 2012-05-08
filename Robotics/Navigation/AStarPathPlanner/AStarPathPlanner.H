#ifndef NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H
#define NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <Messages/ImageProc/GenericImageMessage.H>
#include <Messages/Navigation/PathMessage.H>
#include <nrt/Core/Image/Image.H>
#include <nrt/Core/Geometry/Point.H>
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>

namespace astarpathplanner
{
  NRT_DECLARE_MESSAGECHECKER_PORT(OccupancyMap, GenericImageMessage, "The occupancy map represented as a grayscale image.");
  NRT_DECLARE_MESSAGECHECKER_PORT(PixelsPerMeter, nrt::Message<nrt::real>, "How many pixels per meter.");
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformLookup, nrt::TransformLookupMessage, nrt::TransformMessage, "The transform lookup port");
  NRT_DECLARE_MESSAGEPOSTER_PORT(NextTransform, nrt::TransformMessage, void, "The next position the robot should go to.");
  NRT_DECLARE_MESSAGEPOSTER_PORT(ComputedPath, nrt::PathMessage, void, "The computed path as a vector of 3d points.");
  NRT_DECLARE_MESSAGEPOSTER_PORT(PathImage, GenericImageMessage, void, "The occupancy map with a path drawn on it.");

  static const nrt::ParameterCategory ParamCat("AStarPathPlanner related options");
  static const nrt::ParameterDef<std::string> NextTransformParam("transform", "The name of the transform to update.", "carrot");
  static const nrt::ParameterDef<std::string> FromTransformParam("robot", "The name of the robot's transform.", "robot");
  static const nrt::ParameterDef<std::string> ToTransformParam("goal", "The name of the transform we are trying to reach.", "goal");
  static const nrt::ParameterDef<float> UpdateRateParam("updateRate", "Number of times per second to perform an update.", 10.0);
  static const nrt::ParameterDef<float> SegmentLengthParam("segmentLength", "Number of meters along the path to place the next transform.", 0.2);
}

class AStarPathPlannerModule : public nrt::Module,
  public nrt::MessageChecker<astarpathplanner::OccupancyMap, astarpathplanner::PixelsPerMeter>,
  public nrt::MessagePoster<astarpathplanner::TransformLookup, astarpathplanner::NextTransform, astarpathplanner::ComputedPath, astarpathplanner::PathImage>
{
  public:
    AStarPathPlannerModule(std::string const& instanceName="");

    void run();

    nrt::Transform3D lookupTransform(std::string from, std::string to);

    std::vector<nrt::Point2D<int>> AStar(nrt::Image<nrt::PixGray<nrt::byte>> const mapInput, nrt::Point2D<int> start, nrt::Point2D<int> goal);

  private:
    std::mutex itsMtx;
    nrt::Parameter<std::string> itsNextTransformParam;
    nrt::Parameter<std::string> itsFromTransformParam;
    nrt::Parameter<std::string> itsToTransformParam;
    nrt::Parameter<float> itsUpdateRateParam;
    nrt::Parameter<float> itsSegmentLengthParam;
};

#endif // NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H
