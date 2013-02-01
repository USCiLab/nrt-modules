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

  itsServer.registerProcedure("org.nrtkit.designer/get/blackboard_federation_summary",
      std::bind(&DesignerServerModule::callback_BlackboardFederationSummaryRequest, this, std::placeholders::_1));

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
std::string DesignerServerModule::callback_BlackboardFederationSummaryRequest(rapidjson::Document const & message)
{
  std::lock_guard<std::mutex> _(itsMtx);
  if(itsLastFederationUpdate) {
    return toJSON(*itsLastFederationUpdate);
  } else NRT_WARNING("No Federation Summary Available");
}

NRT_REGISTER_MODULE(DesignerServerModule);
