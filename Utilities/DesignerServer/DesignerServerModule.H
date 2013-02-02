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
  
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT_SPECIAL( GUIdataInput,
      nrt::GUIdataMessage, void, "GUIdata Recieved" );
  
  NRT_DECLARE_MESSAGEPOSTER_PORT_SPECIAL( ModuleLoaderRefresh,
    nrt::BBNickTrigger, nrt::LoaderSummaryMessage, "Refresh the list of modules available on a loader");
}


class DesignerServerModule : public nrt::Module,
                             public nrt::MessageSubscriber<designerserver::BlackboardFederationSummary,
                                                           designerserver::ModuleParamChanged,
                                                           designerserver::GUIdataInput>,
                             public nrt::MessagePoster<designerserver::ModuleLoaderRefresh>
{
  public:
    DesignerServerModule(std::string const & instanceName = "");
    ~DesignerServerModule();

    void onMessage(designerserver::BlackboardFederationSummary m);
    void onMessage(designerserver::ModuleParamChanged m);
    void onMessage(designerserver::GUIdataInput m);
    
    void run();

    std::string callback_BlackboardFederationSummaryRequest(rapidjson::Document const & message);
    std::string callback_GUIDataRequest(rapidjson::Document const & message);
    std::string callback_LoaderSummaryRequest(rapidjson::Document const & message);
    

    // std::function<void(rapidjson::Document const&)> itsCallback;

  private:
    std::shared_ptr<nrt::blackboard::BlackboardFederationSummary const> itsLastFederationUpdate;
    std::shared_ptr<nrt::GUIdataMessage const> itsLastGUIUpdate;
  
    WampServer &itsServer;
    std::mutex itsMtx;


};

#endif // NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H
