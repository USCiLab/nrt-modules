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
  
  itsServer.registerProcedure("org.nrtkit.designer/get/blackboard_federation_summary",
      std::bind(&DesignerServerModule::callback_BlackboardFederationSummaryRequest, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/get/gui_data",
      std::bind(&DesignerServerModule::callback_GUIDataRequest, this, std::placeholders::_1));
  
  itsServer.registerProcedure("org.nrtkit.designer/get/prototypes",
      std::bind(&DesignerServerModule::callback_LoaderSummaryRequest, this, std::placeholders::_1));
  
  itsServer.registerProcedure("org.nrtkit.designer/update/module_position",
      std::bind(&DesignerServerModule::callback_ModulePositionUpdated, this, std::placeholders::_1));
  
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
{ }

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
  NRT_INFO("Got GUI data");  
  if(itsLastSentGUIdataPtr == m.get())
  {
    NRT_INFO("Skipping self-sent data");
    return;
  }
  
  std::cout << "---------- RECEIVING GUI DATA -------------" << std::endl;
  for(auto gd : m->guidata)
    std::cout << gd.first << " (" << gd.second.x << ", " << gd.second.y << ")" << std::endl; 
  std::cout << "-------------------------------------------" << std::endl;
  
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
      if(result.ready())
      {
        loaderSummary = result.get();
        if(loaderSummary->bbNick != bbnick) continue;
      }
      else
      {
        std::cerr << "Result not ready...\n";
        usleep(1000);
      }
      
    }
  } catch(std::exception &ex) {
    std::cout << ex.what() << std::endl;
    
    throw WampRPCException("org.nrtkit.designer/error/prototype_load", "NRT could not get the bbnick", "\"" + bbnick + "\"");
  }
  
  if(loaderSummary)
    return toJSON(*loaderSummary);
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
    
      std::cout << "Got " << moduid << " " << x << ", " << y << std::endl;
    
      // Prepare a GUIdata object
      std::string key = "m:" + moduid;
      nrt::blackboard::GUIdata gd = nrt::blackboard::GUIdata();
      gd.x = x;
      gd.y = y;
    
      // Prepare the GUIdataMessage
      payload->guidata[key] = gd;
    
      NRT_INFO("Posting GUI data");
      // Away it goes
    } else {
      throw WampRPCException("org.nrtkit.designer/error/module_position_args", "Invalid object");
    }
      
  }
  
  std::cout << "---------- SENDING GUI DATA -------------" << std::endl;
  for(auto gd : payload->guidata)
    std::cout << gd.first << " (" << gd.second.x << ", " << gd.second.y << ")" << std::endl; 
  std::cout << "-----------------------------------------" << std::endl;
  
  itsLastSentGUIdataPtr = payload.get();
  post<designerserver::GUIdataOutput>(payload);
    
  std::cout << "Passed" << std::endl;
    
  return "";
    
}

NRT_REGISTER_MODULE(DesignerServerModule);
