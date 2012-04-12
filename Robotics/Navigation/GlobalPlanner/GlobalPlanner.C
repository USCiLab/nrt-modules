#include "GlobalPlanner.H"

using namespace nrt;
using namespace globalplanner;

// ######################################################################
GlobalPlannerModule::GlobalPlannerModule(std::string const& instanceName) :
  Module(instanceName),
  itsNextTransformParam(NextTransformParam, this)
{
  //
}

void GlobalPlannerModule::onMessage(globalplanner::OccupancyMap map)
{
}

void GlobalPlannerModule::run()
{
  while(running())
  {
    //
  }
}
