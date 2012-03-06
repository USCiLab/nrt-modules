#include "TransformManagerModule.H"

using namespace transformmanager;

// ######################################################################
TransformManagerModule::TransformManagerModule( std::string const & instanceName ) :
  nrt::Module( instanceName ),
  itsMaxCacheSize( transformmanager::MaxCacheSizeParamDef, this ),
  itsMaxCacheTime( transformmanager::MaxCacheTimeParamDef, this )
{ }

// ######################################################################
void TransformManagerModule::run()
{ }

// ######################################################################
void TransformManagerModule::onMessage( transformmanager::TransformUpdatePort msg )
{
  itsTransformManager.updateTransform( msg );
}

// ######################################################################
TransformLookupPort::RetPtr TransformManagerModule::onMessage(TransformLookupPort msg)
{
  return itsTransformManager.lookupTransform( msg );
}

NRT_REGISTER_MODULE(TransformManagerModule);

