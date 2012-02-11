#include "DeadReckoningModule.H"
#include <SerialPort.h>
#include <nrt/Core/Util/MathUtils.H>

using namespace nrt;
using namespace deadreckoning; 

// ######################################################################
DeadReckoningModule::DeadReckoningModule(std::string const& instanceName) :
  Module(instanceName),
  itsBaseFrameParam(BaseFrameParamDef, this),
  itsDeadReckoningFrameParam(OutputFrameParamDef, this)
{ }


// ######################################################################
void DeadReckoningModule::onMessage(VelocityCommand msg)
{
  std::lock_guard<std::mutex> _(itsMtx);

  // Uh oh! We can't handle full screw movement right now, so let's throw an exception if
  // users are trying to mess around with other velocities
  if(msg->linear.y() != 0 || msg->linear.z() != 0 || msg->angular.x() != 0 || msg->angular.y() != 0)
    throw NRT_MODULE_EXCEPTION("Only linear.x and angular.z velocities are currently supported"); 


  if(itsLastUpdateTime != nrt::Time())
  {
  }

  itsLastVelocity   = *msg;
  itsLastUpdateTime = nrt::now();
}

NRT_REGISTER_MODULE(DeadReckoningModule);



