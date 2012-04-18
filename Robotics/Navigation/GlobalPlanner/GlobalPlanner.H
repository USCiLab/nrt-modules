#ifndef NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H
#define NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <Messages/ImageProc/GenericImageMessage.H>
#include <nrt/Core/Image/Image.H>
#include <nrt/Core/Geometry/Point.H>
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>

namespace globalplanner
{
  NRT_DECLARE_MESSAGECHECKER_PORT(OccupancyMap, GenericImageMessage, "The occupancy map represented as a grayscale image.");
  NRT_DECLARE_MESSAGECHECKER_PORT(PixelsPerMeter, nrt::Message<nrt::real>, "How many pixels per meter.");
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformLookup, nrt::TransformLookupMessage, nrt::TransformMessage, "The transform lookup port");
  NRT_DECLARE_MESSAGEPOSTER_PORT(NextTransform, nrt::TransformMessage, void, "The next position the robot should go to.");

  static const nrt::ParameterCategory ParamCat("GlobalPlanner related options");

  static const nrt::ParameterDef<std::string> NextTransformParam("transform", "The name of the transform to update.", "carrot");

  static const nrt::ParameterDef<std::string> FromTransformParam("robot", "The name of the robot's transform.", "robot");

  static const nrt::ParameterDef<std::string> ToTransformParam("goal", "The name of the transform we are trying to reach.", "goal");

  static const nrt::ParameterDef<float> UpdateRateParam("updateRate", "Number of times per second to perform an update.", 10.0);

  static const nrt::ParameterDef<float> SegmentLengthParam("segmentLength", "Number of meters along the path to place the next transform.", 0.2);

  static const nrt::ParameterDef<bool> ShowDebugParam("showDebugMap", "Show the debug map.", false);
}

class GlobalPlannerModule : public nrt::Module,
  public nrt::MessageChecker<globalplanner::OccupancyMap, globalplanner::PixelsPerMeter>,
  public nrt::MessagePoster<globalplanner::TransformLookup, globalplanner::NextTransform>
{
  public:
    GlobalPlannerModule(std::string const& instanceName="");

    void run();

    nrt::Transform3D lookupTransform(std::string from, std::string to);

    std::vector<nrt::Point2D<int>> AStar(nrt::Image<nrt::PixGray<nrt::byte>> const mapInput, nrt::Point2D<int> start, nrt::Point2D<int> goal);

  private:
    std::mutex itsMtx;
    std::shared_ptr<nrt::DisplayImageSink> itsDisplay;
    nrt::Parameter<std::string> itsNextTransformParam;
    nrt::Parameter<std::string> itsFromTransformParam;
    nrt::Parameter<std::string> itsToTransformParam;
    nrt::Parameter<float> itsUpdateRateParam;
    nrt::Parameter<float> itsSegmentLengthParam;
    nrt::Parameter<bool> itsShowDebugParam;

    void debugParamCallback(bool const & debug);
};

#endif // NRT_MODULES_ROBOTICS_NAVIGATION_GLOBALPLANNER_H
