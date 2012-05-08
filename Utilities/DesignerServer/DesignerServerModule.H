#ifndef NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H
#define NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H

#include <nrt/Core/Blackboard/ModuleLoaderMessages.H>
#include <nrt/Core/Blackboard/Module.H>
#include "Server.H"

namespace designerserver
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT_SPECIAL( BlackboardFederationSummary,
      nrt::blackboard::BlackboardFederationSummary, void, "Blackboard Federation Summary" );
}


class DesignerServerModule : public nrt::Module,
                             public nrt::MessageSubscriber<designerserver::BlackboardFederationSummary>
{
  public:
    DesignerServerModule(std::string const & instanceName = "");
    ~DesignerServerModule();
    void onMessage(designerserver::BlackboardFederationSummary m);

    void run();

  private:
    Server itsServer;
};

#endif // NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H
