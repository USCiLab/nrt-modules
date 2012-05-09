#include "DesignerServerModule.H"
#include "MessageSerializers.H" 

using namespace nrt;
using namespace designerserver;

// ######################################################################
DesignerServerModule::DesignerServerModule(std::string const & instanceName) :
  Module(instanceName)
{
  setSubscriberTopicFilter<BlackboardFederationSummary>(".*");

  itsServer.registerCallback("BlackboardFederationSummaryRequest", std::bind(&DesignerServerModule::callback_BlackboardFederationSummaryRequest, this, std::placeholders::_1));

  itsServer.start(8080);
}

// ######################################################################
DesignerServerModule::~DesignerServerModule()
{
  itsServer.stop();
}

// ######################################################################
void DesignerServerModule::onMessage(BlackboardFederationSummary m)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsLastFederationSummary = *m;
  itsServer.broadcastMessage(toJSON(*m));
}

// ######################################################################
void DesignerServerModule::run()
{
  while(running())
  {
    usleep(100000);
  }
}

// ######################################################################
void DesignerServerModule::callback_BlackboardFederationSummaryRequest(rapidjson::Document const & message)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsServer.broadcastMessage(toJSON(itsLastFederationSummary));
}

NRT_REGISTER_MODULE(DesignerServerModule);
