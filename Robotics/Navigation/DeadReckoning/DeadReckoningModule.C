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

  if(itsLastUpdateTime != nrt::Time())
  {
  }

  itsLastVelocity = *msg;
  itsLastUpdateTime = nrt::now();
}

NRT_REGISTER_MODULE(DeadReckoningModule);



