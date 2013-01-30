#ifndef NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H
#define NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H

#include <nrt/Core/Blackboard/ModuleLoaderMessages.H>
#include <nrt/Core/Blackboard/Module.H>
#include "libs/rapidjson/document.h"
#include "WampSession.h"
#include "WampServer.h"

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

    std::string callback_BlackboardFederationSummaryRequest(rapidjson::Document const & message);

    // std::function<void(rapidjson::Document const&)> itsCallback;

  private:
    std::shared_ptr<nrt::blackboard::BlackboardFederationSummary const> itsLastFederationUpdate;
  
    WampServer &itsServer;
    std::mutex itsMtx;


};

#endif // NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H
