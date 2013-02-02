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

  itsServer.registerProcedure("org.nrtkit.designer/get/blackboard_federation_summary",
      std::bind(&DesignerServerModule::callback_BlackboardFederationSummaryRequest, this, std::placeholders::_1));

  itsServer.registerProcedure("org.nrtkit.designer/get/gui_data",
      std::bind(&DesignerServerModule::callback_GUIDataRequest, this, std::placeholders::_1));
  
  itsServer.registerProcedure("org.nrtkit.designer/get/prototypes",
      std::bind(&DesignerServerModule::callback_LoaderSummaryRequest, this, std::placeholders::_1));
  
  itsServer.start(8080);
}

// ######################################################################
DesignerServerModule::~DesignerServerModule()
{
  itsServer.stop();
}

// ######################################################################
void DesignerServerModule::run()
{
  while(running()) usleep(10000);
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
  std::lock_guard<std::mutex> _(itsMtx);
  itsLastGUIUpdate = m;
  itsServer.broadcastEvent("org.nrtkit.designer/event/gui_data_input", toJSON(*m));
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
    loaderSummary = post<designerserver::ModuleLoaderRefresh>(trigger).get();
  } catch(const std::exception &e) {
    std::cout << "Exception :: " << e.what() << std::endl;
    throw WampRPCException("org.nrtkit.designer/error/prototype_load", "NRT could not get the bbnick", "\"" + bbnick + "\"");
  }
  
  if(loaderSummary)
    return toJSON(*loaderSummary);
  else NRT_WARNING("No Loader Summary Message returned");
  
  return "";
}


NRT_REGISTER_MODULE(DesignerServerModule);
