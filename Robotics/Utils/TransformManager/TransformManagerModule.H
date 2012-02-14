#ifndef NRT_ROBOTICS_MODULES_TRANSFORMMANAGERMODULE_H_
#define NRT_ROBOTICS_MODULES_TRANSFORMMANAGERMODULE_H_

#include <nrt/Robotics/Messages/TransformMessage.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/TransformManager.H>

#define NRT_DECLARE_MESSAGE_CALLBACK( PortName ) \
PortName::RetPtr onMessage( PortName )

#define NRT_IMPL_MESSAGE_CALLBACK( namespace_name, Class, PortName ) \
namespace_name::PortName::RetPtr \
  namespace_name::Class::onMessage( PortName msg )

namespace nrt
{
  namespace transformmanager
  {
    static const nrt::ParameterCategory TransformMgrParamCateg( "TransformManager Options" );
    static const nrt::ParameterDef<uint64_t> MaxCacheSizeParamDef( "maxCacheSize", "number of transforms to store before discarding old transforms", 1000, TransformMgrParamCateg );
    static const nrt::ParameterDef<double> MaxCacheTimeParamDef( "maxCacheTime", "amount of time (in seconds) to wait before discarding old transforms", 30.0, TransformMgrParamCateg );

    NRT_DECLARE_MESSAGESUBSCRIBER_PORT( TransformUpdatePort, nrt::TransformMessage, void, "Incoming transforms to cache" );
    NRT_DECLARE_MESSAGESUBSCRIBER_PORT( TransformLookupPort, nrt::TransformLookupMessage, nrt::TransformMessage, "Result of a transform lookup" );
  }

  class TransformManagerModule : public nrt::Module,
                                 public nrt::MessageSubscriber<transformmanager::TransformUpdatePort, transformmanager::TransformLookupPort>
  {
    public:
      TransformManagerModule( std::string const & instanceName = "" );

      virtual void run();

      void onMessage( transformmanager::TransformUpdatePort msg );

      NRT_DECLARE_MESSAGE_CALLBACK( transformmanager::TransformLookupPort );
      
    private:
      nrt::Parameter<uint64_t> itsMaxCacheSize;
      nrt::Parameter<double> itsMaxCacheTime;
      TransformManager itsTransformManager;
  };
}

#endif // NRT_ROBOTICS_MODULES_TRANSFORMMANAGERMODULE_H_
