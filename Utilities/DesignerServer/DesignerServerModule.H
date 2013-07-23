#ifndef NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H
#define NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H

#include <nrt/Core/Blackboard/ModuleLoaderMessages.H>
#include <nrt/Core/Blackboard/Module.H>
#include "libs/rapidjson/document.h"
#include <nrt/Core/Blackboard/BlackboardManagerMessages.H>
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

  NRT_DECLARE_MESSAGEPOSTER_PORT_SPECIAL(GUIdataOutput,
    nrt::GUIdataMessage, void, "GUI Data Changes Output")

  NRT_DECLARE_MESSAGEPOSTER_PORT_SPECIAL (LoadModule,
    nrt::LoadModuleRequestMessage, nrt::LoadModuleResponseMessage, "Request module load")

  NRT_DECLARE_MESSAGEPOSTER_PORT_SPECIAL (UnloadModule,
    nrt::UnloadModuleMessage, void, "Unload module")

  NRT_DECLARE_MESSAGEPOSTER_PORT_SPECIAL (ModifyModuleTopic,
    nrt::ModifyModuleTopicMessage, nrt::Message<bool>, "Modify module topic")

  NRT_DECLARE_MESSAGEPOSTER_PORT_SPECIAL (ModifyModuleParam,
    nrt::ModifyParamMessage, nrt::ModifyParamResponseMessage, "Modify module parameter")

  NRT_DECLARE_MESSAGEPOSTER_PORT_SPECIAL (GetModuleParam,
    nrt::GetParamMessage, nrt::GetParamResponseMessage, "Modify module parameter")

  NRT_DECLARE_MESSAGEPOSTER_PORT_SPECIAL (StartStopNRT,
    nrt::SetStateMessage, void, "Start or stop NRT")

}


class DesignerServerModule : public nrt::Module,
                             public nrt::MessageSubscriber<designerserver::BlackboardFederationSummary,
                                                           designerserver::ModuleParamChanged,
                                                           designerserver::GUIdataInput>,
                             public nrt::MessagePoster<designerserver::ModuleLoaderRefresh,
                                                       designerserver::GUIdataOutput,
                                                       designerserver::LoadModule,
                                                       designerserver::UnloadModule,
                                                       designerserver::ModifyModuleTopic,
                                                       designerserver::StartStopNRT,
                                                       designerserver::ModifyModuleParam,
                                                       designerserver::GetModuleParam>
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
    std::string callback_ModulePositionUpdated(rapidjson::Document const & message);
    std::string callback_CreateModule(rapidjson::Document const & message);
    std::string callback_DeleteModule(rapidjson::Document const & message);
    std::string callback_EditModuleTopic(rapidjson::Document const & message);
    std::string callback_StartStopNRT(rapidjson::Document const & message);
    std::string callback_EditParameter(rapidjson::Document const & message);
    std::string callback_GetParameter(rapidjson::Document const & message);


    // std::function<void(rapidjson::Document const&)> itsCallback;

  private:
    std::shared_ptr<nrt::blackboard::BlackboardFederationSummary const> itsLastFederationUpdate;
    std::shared_ptr<nrt::GUIdataMessage const> itsLastGUIUpdate;

    // WARNING! Don't try to do anything with the data pointed to here. It's only to check if we're sending data to ourselves or not... Sorry!
    nrt::GUIdataMessage * itsLastSentGUIdataPtr;

    WampServer &itsServer;
    std::mutex itsMtx;


};

#endif // NRT_MODULES_UTILITIES_DESIGNERSERVER_DESIGNERSERVERMODULE_H
