#include "PathFollower.H"
#include <nrt/ImageProc/Drawing/Geometry.H>

using namespace nrt;
using namespace pathfollower;

// ######################################################################
PathFollowerModule::PathFollowerModule(std::string const & instanceName) :
  Module(instanceName),
  itsFromTransformParam(FromTransformParam, this),
  itsToTransformParam(ToTransformParam, this),
  itsShowDebugParam(ShowDebugParam, this, &PathFollowerModule::debugParamCallback)
{
}

// ######################################################################
PathFollowerModule::onMessage(Path msg)
{
  // get the coords of the robot relative to the 
}
