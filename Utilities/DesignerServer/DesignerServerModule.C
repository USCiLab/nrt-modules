#include "DesignerServerModule.H"
#include "MessageSerializers.H" 

using namespace nrt;
using namespace designerserver;

// ######################################################################
DesignerServerModule::DesignerServerModule(std::string const & instanceName) :
  Module(instanceName)
{
  setSubscriberTopicFilter<BlackboardFederationSummary>(".*");
  setSubscriberTopicFilter<ModuleParamChanged>(".*");

  itsServer.registerCallback("BlackboardFederationSummaryRequest",
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
  itsServer.broadcastMessage(toJSON(*m));
}

// ######################################################################
void DesignerServerModule::onMessage(designerserver::ModuleParamChanged m)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsServer.broadcastMessage(toJSON(*m));
}

// ######################################################################
void DesignerServerModule::callback_BlackboardFederationSummaryRequest(rapidjson::Document const & message)
{
  std::lock_guard<std::mutex> _(itsMtx);
  if(itsLastFederationUpdate)
    itsServer.broadcastMessage(toJSON(*itsLastFederationUpdate));
  else NRT_WARNING("No Federation Summary Available");
}

NRT_REGISTER_MODULE(DesignerServerModule);
