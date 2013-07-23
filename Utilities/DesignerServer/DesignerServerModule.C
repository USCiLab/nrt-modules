#include "DesignerServerModule.H"
#include "MessageSerializers.H"

using namespace nrt;
using namespace designerserver;

// ######################################################################
DesignerServerModule::DesignerServerModule(std::string const & instanceName) :
  Module(instanceName),
  itsServer(WampServer::getInstance())
{
  setSubscriberTopicFilter<BlackboardFederationSummary>(".*");
  setSubscriberTopicFilter<ModuleParamChanged>(".*");
  setSubscriberTopicFilter<GUIdataInput>(".*");

  setPosterTopic<ModuleLoaderRefresh>("NRT_RequestLoaderSummary");
  setPosterTopic<GUIdataOutput>("NRT_SetGUIdata");
  setPosterTopic<LoadModule>("NRT_LoadModule");
  setPosterTopic<UnloadModule>("NRT_UnloadModule");
  setPosterTopic<ModifyModuleTopic>("NRT_ModifyTopic");
  setPosterTopic<StartStopNRT>("NRT_SetState");
  setPosterTopic<ModifyModuleParam>("NRT_ModifyParam");
  setPosterTopic<GetModuleParam>("NRT_GetParam");


  itsServer.registerProcedure("org.nrtkit.designer/get/blackboard_federation_summary",
      std::bind(&DesignerServerModule::callback_BlackboardFederationSummaryRequest, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/get/gui_data",
      std::bind(&DesignerServerModule::callback_GUIDataRequest, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/get/prototypes",
      std::bind(&DesignerServerModule::callback_LoaderSummaryRequest, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/update/module_position",
      std::bind(&DesignerServerModule::callback_ModulePositionUpdated, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/post/module",
      std::bind(&DesignerServerModule::callback_CreateModule, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/delete/module",
      std::bind(&DesignerServerModule::callback_DeleteModule, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/edit/module/topic",
      std::bind(&DesignerServerModule::callback_EditModuleTopic, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/edit/nrt",
      std::bind(&DesignerServerModule::callback_StartStopNRT, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/edit/parameter",
      std::bind(&DesignerServerModule::callback_EditParameter, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/get/parameter",
      std::bind(&DesignerServerModule::callback_GetParameter, this, std::placeholders::_1));

  itsServer.start(8080);
}

// ######################################################################
DesignerServerModule::~DesignerServerModule()
{
  NRT_INFO("Stopping Server");
  itsServer.stop();
}

// ######################################################################
void DesignerServerModule::run()
{
  std::cout << "Run" << std::endl;
}

// ######################################################################
void DesignerServerModule::onMessage(BlackboardFederationSummary m)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsLastFederationUpdate = m;
  itsServer.broadcastEvent("org.nrtkit.designer/event/blackboard_federation_summary", toJSON(*m));
}

// ######################################################################
void DesignerServerModule::onMessage(designerserver::ModuleParamChanged m)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsServer.broadcastEvent("org.nrtkit.designer/event/module_param_update", toJSON(*m));
}

// ######################################################################
void DesignerServerModule::onMessage(designerserver::GUIdataInput m)
{
  if(itsLastSentGUIdataPtr == m.get())
  {
    return;
  }

  std::lock_guard<std::mutex> _(itsMtx);
  itsLastGUIUpdate = m;
  itsServer.broadcastEvent("org.nrtkit.designer/event/gui_data_update", toJSON(*m));
}

// ######################################################################
std::string DesignerServerModule::callback_BlackboardFederationSummaryRequest(rapidjson::Document const & message)
{
  std::lock_guard<std::mutex> _(itsMtx);
  if(itsLastFederationUpdate) {
    return toJSON(*itsLastFederationUpdate);
  } else NRT_WARNING("No Federation Summary Available");

  return "";
}

std::string DesignerServerModule::callback_GUIDataRequest(rapidjson::Document const & message)
{
  std::lock_guard<std::mutex> _(itsMtx);
  if(itsLastGUIUpdate) {
    return toJSON(*itsLastGUIUpdate);
  } else NRT_WARNING("No GUI Data Available");

  return "";
}

std::string DesignerServerModule::callback_LoaderSummaryRequest(rapidjson::Document const & message)
{
  std::lock_guard<std::mutex> _(itsMtx);

  if(message.Capacity() != 4) {
    throw WampRPCException("org.nrtkit.designer/error/prototype_load_args", "Bad argument list");
  }

  std::string const bbnick = message[3u].GetString(); // Get the bbnick from the message
  std::cout << "\n\n" << bbnick << "\n\n";
  std::unique_ptr<nrt::BBNickTrigger> trigger(new nrt::BBNickTrigger);
  trigger->bbNick = bbnick;

  std::shared_ptr<nrt::LoaderSummaryMessage const> loaderSummary;
  try {
    auto result = post<designerserver::ModuleLoaderRefresh>(trigger);
    while ( !result.empty() ) {

      std::shared_ptr<nrt::LoaderSummaryMessage const> currSummary = result.get();
      NRT_INFO("Loader summary request GOT: " << currSummary->bbNick);
      if(currSummary->bbNick == bbnick)
      {
        loaderSummary = currSummary;
        break;
      }

    }
  } catch(std::exception &ex) {
    std::cout << ex.what() << std::endl;

    throw WampRPCException("org.nrtkit.designer/error/prototype_load", "NRT could not get the bbnick", "\"" + bbnick + "\"");
  }

  if(loaderSummary)
  {
    NRT_INFO("GOT LOADER SUMMARY FROM " << bbnick << " with " << loaderSummary->modules.size() << " modules");
    return toJSON(*loaderSummary);
  }
  else NRT_WARNING("No Loader Summary Message returned");

  return "";
}

std::string DesignerServerModule::callback_ModulePositionUpdated(rapidjson::Document const & message)
{
  std::unique_ptr<nrt::GUIdataMessage> payload(new nrt::GUIdataMessage);

  {
    std::lock_guard<std::mutex> _(itsMtx);

    if(message.Capacity() != 4){
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
    }

    if(!message[3u].IsObject()) {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
    }

    if(message[3u].HasMember("moduid") && message[3u].HasMember("x") && message[3u].HasMember("y")) {
      std::string moduid = message[3u]["moduid"].GetString();
      int x = message[3u]["x"].GetInt();
      int y = message[3u]["y"].GetInt();

      // Prepare a GUIdata object
      std::string key = "m:" + moduid;
      nrt::blackboard::GUIdata gd = nrt::blackboard::GUIdata();
      gd.x = x;
      gd.y = y;

      // Prepare the GUIdataMessage
      payload->guidata[key] = gd;

      NRT_INFO("Posting GUI data " << key << " (" << x << ", " << y << ")");
      // Away it goes
    } else {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Invalid object");
    }

  }

  itsLastSentGUIdataPtr = payload.get();
  post<designerserver::GUIdataOutput>(payload);

  return "";

}

std::string DesignerServerModule::callback_CreateModule(rapidjson::Document const & message)
{
  std::unique_ptr<nrt::LoadModuleRequestMessage> payload(new nrt::LoadModuleRequestMessage);

  {
    std::lock_guard<std::mutex> _(itsMtx);

    if(message.Capacity() != 4){
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
    }

    if(!message[3u].IsObject()) {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
    }

    if(message[3u].HasMember("logicalPath") && message[3u].HasMember("bbNick")) {
      payload->bbNick = message[3u]["bbNick"].GetString();
      payload->logicalPath = message[3u]["logicalPath"].GetString();
      payload->instanceName = "NRT_AUTO_#";
      payload->ns = "/";

      // Away it goes
    } else {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Invalid object");
    }

  }

  // Create the module
  std::shared_ptr<nrt::LoadModuleResponseMessage const> response;

  try
  {
    auto results = post<designerserver::LoadModule>(payload);
    while ( !results.empty() ) {
      response = results.get();
      NRT_INFO("Response: " << response->className << " " << response->moduleUID);
      if(response->moduleUID == "")
        continue;

      return "\"" + response->moduleUID + "\"";
    }
  }
  catch(nrt::exception::ModuleException const & e)
  { throw WampRPCException("org.nrtkit.designer/error/create_module_failed", e.what()); }
  catch(nrt::exception::BlackboardException const & e)
  { throw WampRPCException("org.nrtkit.designer/error/create_module_failed", e.what()); }

  NRT_INFO("Well go on then...");
  throw WampRPCException("org.nrtkit.designer/error/create_module_failed", "Loader with given bbnick did not respond");
}

std::string DesignerServerModule::callback_DeleteModule(rapidjson::Document const & message)
{
  std::unique_ptr<nrt::UnloadModuleMessage> payload(new nrt::UnloadModuleMessage);

  {
    std::lock_guard<std::mutex> _(itsMtx);

    if(message.Capacity() != 4){
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
    }

    if(!message[3u].IsObject()) {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
    }

    if(message[3u].HasMember("moduid")) {
      payload->uid = message[3u]["moduid"].GetString();

      // Away it goes
    } else {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Invalid object");
    }

  }

  // Delete the module
  post<designerserver::UnloadModule>(payload);

  NRT_INFO("Module Deleted.");
  return "";
}

std::string DesignerServerModule::callback_EditModuleTopic(rapidjson::Document const & message)
{
  std::unique_ptr<nrt::ModifyModuleTopicMessage> payload(new nrt::ModifyModuleTopicMessage);

  {
    std::lock_guard<std::mutex> _(itsMtx);

    if(message.Capacity() != 4){
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
    }

    if(!message[3u].IsObject()) {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
    }

    if(message[3u].HasMember("moduid")
      && message[3u].HasMember("port_type")
      && message[3u].HasMember("portname")
      && message[3u].HasMember("topi"))
    {
      payload->moduleUID = message[3u]["moduid"].GetString();
      payload->portName = message[3u]["portname"].GetString();
      payload->topi = message[3u]["topi"].GetString();

      std::string port_type = message[3u]["port_type"].GetString();
      if(port_type == "input")
        payload->portType = nrt::ModifyModuleTopicMessage::PortType::ModuleSubscriber;
      else if(port_type == "output")
        payload->portType = nrt::ModifyModuleTopicMessage::PortType::ModulePoster;
      else
        throw WampRPCException("org.nrtkit.designer/error/argument_error", "Bad port type");

      NRT_INFO("Payload: ");
      NRT_INFO("Moduid: " << payload->moduleUID);
      NRT_INFO("Portname: " << payload->portName);
      NRT_INFO("Topic: " << payload->topi);
      NRT_INFO("Port type: " << payload->portType);

      // Away it goes
    } else {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Invalid object");
    }

  }

  // Edit the module topic
  post<designerserver::ModifyModuleTopic>(payload);

  NRT_INFO("Connection Created.");
  return "";
}

std::string DesignerServerModule::callback_StartStopNRT(rapidjson::Document const & message)
{

  std::unique_ptr<nrt::SetStateMessage> payload(new nrt::SetStateMessage);

  {
    std::lock_guard<std::mutex> _(itsMtx);

    NRT_INFO(message.Capacity());

    if(message.Capacity() != 4){
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
    }

    if(!message[3u].IsString()) {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Argument is not string");
    }

    const char* msg = message[3u].GetString();
    if(strcmp(msg, "start") == 0) {
      NRT_INFO("Start");
      payload->state = nrt::SetStateMessage::State::Launch;
    } else if(strcmp(msg, "stop") == 0) {
      NRT_INFO("Stop");
      payload->state = nrt::SetStateMessage::State::Stop;
    } else {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Unrecognize message");
    }

  }

  // Send msg
  post<designerserver::StartStopNRT>(payload);

  return "";
}

std::string DesignerServerModule::callback_EditParameter(rapidjson::Document const & message)
{

  if(message.Capacity() != 4){
    throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
  }

  auto const & json_request = message[3u];

  if(!json_request.IsObject()) {
    throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
  }

  if(! (json_request.HasMember("parameter_descriptor") &&
        json_request.HasMember("parameter_value") &&
        json_request.HasMember("module_uid")) )
    throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");

  auto const & parameter_descriptor = json_request["parameter_descriptor"].GetString();
  auto const & parameter_value      = json_request["parameter_value"].GetString();
  auto const & module_uid           = json_request["module_uid"].GetString();

  std::unique_ptr<nrt::ModifyParamMessage> payload(new nrt::ModifyParamMessage);
  payload->paramName  = parameter_descriptor;
  payload->paramValue = parameter_value;
  payload->moduleUID  = module_uid;

  try
  {
    bool success = false;
    NRT_INFO("Changing parameter [" << parameter_descriptor << "] [" << parameter_value << "] [" << module_uid << "]");
    auto results = post<ModifyModuleParam>(payload);
    while(!results.empty()) 
      success |= results.get()->success;
    NRT_INFO("   Success: " << success);
  }
  catch(nrt::exception::ModuleException e)
  {
    throw WampRPCException("org.nrtkit.designer/error/set_parameter_failed", e.bbwhat(0), e.str());
  }
  catch(nrt::exception::BlackboardException e)
  {
    throw WampRPCException("org.nrtkit.designer/error/set_parameter_failed", e.bbwhat(0), e.str());
  }
  catch(...)
  {
    throw WampRPCException("org.nrtkit.designer/error/set_parameter_failed", "Unknown Error");
  }

  return "";
}

std::string DesignerServerModule::callback_GetParameter(rapidjson::Document const & message)
{

  if(message.Capacity() != 4){
    throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
  }

  auto const & json_request = message[3u];

  if(!json_request.IsObject()) {
    throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");
  }

  if(! (json_request.HasMember("parameter_descriptor") &&
        json_request.HasMember("module_uid")) )
    throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Bad argument list");

  auto const & parameter_descriptor = json_request["parameter_descriptor"].GetString();
  auto const & module_uid           = json_request["module_uid"].GetString();

  std::unique_ptr<nrt::GetParamMessage> payload(new nrt::GetParamMessage);
  payload->paramName  = parameter_descriptor;
  payload->moduleUID  = module_uid;

  try
  {
    bool success = false;
    NRT_INFO("Getting parameter [" << parameter_descriptor << "] [" << module_uid << "]");
    auto results = post<GetModuleParam>(payload);
    while(!results.empty()) 
    {
      auto result = results.get();
      NRT_INFO("Got result: " << result->valid << " " << result->value);
      if(result->valid)
      {
        return "\"" + result->value + "\"";
      }
    }
    NRT_INFO("    Failed");
  }
  catch(nrt::exception::ModuleException e)
  {
    throw WampRPCException("org.nrtkit.designer/error/get_parameter_failed", e.bbwhat(0), e.str());
  }
  catch(nrt::exception::BlackboardException e)
  {
    throw WampRPCException("org.nrtkit.designer/error/get_parameter_failed", e.bbwhat(0), e.str());
  }
  catch(...)
  {
    throw WampRPCException("org.nrtkit.designer/error/get_parameter_failed", "Unknown Error");
  }

  throw WampRPCException("org.nrtkit.designer/error/get_parameter_failed", "No response");
}

NRT_REGISTER_MODULE(DesignerServerModule);
