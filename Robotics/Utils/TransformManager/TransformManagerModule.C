#include <nrt/Robotics/Modules/TransformManagerModule.H>

// ######################################################################
nrt::TransformManagerModule::TransformManagerModule( std::string const & instanceName ) :
  nrt::Module( instanceName ), itsMaxCacheSize( transformmanager::MaxCacheSizeParamDef, this ), itsMaxCacheTime( transformmanager::MaxCacheTimeParamDef, this )
{
  //
}

// ######################################################################
void
  nrt::TransformManagerModule::run()
{
  //
}

// ######################################################################
void
  nrt::TransformManagerModule::onMessage( transformmanager::TransformUpdatePort msg )
{
  itsTransformManager.updateTransform( msg );
}

// ######################################################################

//nrt::transformmanager::TransformLookupPort::RetPtr nrt::Robotics::TransformManagerModule::onMessage( transformmanager::TransformLookupPort msg )

NRT_IMPL_MESSAGE_CALLBACK( nrt, TransformManagerModule, transformmanager::TransformLookupPort )
{
  return itsTransformManager.lookupTransform( msg );
}
