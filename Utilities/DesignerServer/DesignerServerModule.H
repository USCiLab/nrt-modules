#ifndef NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H
#define NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H

#include <nrt/Core/Blackboard/ModuleLoaderMessages.H>
#include <nrt/Core/Blackboard/Module.H>
#include "Server.H"

namespace designerserver
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT_SPECIAL( BlackboardFederationSummary,
      nrt::blackboard::BlackboardFederationSummary, void, "Blackboard Federation Summary" );

  NRT_DECLARE_MESSAGESUBSCRIBER_PORT_SPECIAL( ModuleParamChanged,
      nrt::ModuleParamChangedMessage, void, "Parameter Changed" );

  
}


class DesignerServerModule : public nrt::Module,
                             public nrt::MessageSubscriber<designerserver::BlackboardFederationSummary,
                                                           designerserver::ModuleParamChanged>
{
  public:
    DesignerServerModule(std::string const & instanceName = "");
    ~DesignerServerModule();

    void onMessage(designerserver::BlackboardFederationSummary m);
    void onMessage(designerserver::ModuleParamChanged m);

    void run();

  private:
  
    Server itsServer;
    std::mutex itsMtx;

    void callback_BlackboardFederationSummaryRequest(rapidjson::Document const & message);

    nrt::blackboard::BlackboardFederationSummary itsLastFederationSummary;
};

#endif // NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H
