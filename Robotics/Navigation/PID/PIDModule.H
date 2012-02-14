#ifndef NRT_ROBOTICS_MODULES_PIDMODULE_H_
#define NRT_ROBOTICS_MODULES_PIDMODULE_H_

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Components/PIDComponent.H>

namespace nrt
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT( DesiredValuePort, nrt::Message<double>, void, "The desired value for the control loop" );
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT( ObservedValuePort, nrt::Message<double>, void, "The raw observed value to be used for the control loop" );
  NRT_DECLARE_MESSAGEPOSTER_PORT( OutputValuePort, nrt::Message<double>, void, "The control loop output" );

  class PIDModule : public nrt::Module,
                    public nrt::MessageSubscriber<DesiredValuePort, ObservedValuePort>,
                    public nrt::MessagePoster<OutputValuePort>
  {
    public:
      PIDModule( std::string const & instanceName = "" );

      virtual void onMessage( DesiredValuePort port );
      virtual void onMessage( ObservedValuePort port );

    private:
      nrt::PIDComponent itsPidComponent;
  };
}

#endif // NRT_ROBOTICS_MODULES_PIDMODULE_H_
