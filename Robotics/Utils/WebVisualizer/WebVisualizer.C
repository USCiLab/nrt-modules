#include "WebVisualizer.H"
#include "mongoose/mongoose.h"
#include <nrt/Core/Util/StringUtil.H>
#include <iostream>
#include <fstream>

using namespace nrt;
using namespace webvisualizer;

// ######################################################################
WebVisualizerModule::WebVisualizerModule(std::string const & instanceName) :
  Module(instanceName),
  itsPort(PortParam, this, &WebVisualizerModule::PortChangeCallback),
  itsDocumentRoot(DocumentRootParam, this, &WebVisualizerModule::DocumentRootChangeCallback)
{ 
}

// ######################################################################
void WebVisualizerModule::PortChangeCallback(int const & port)
{
}

// ######################################################################
void WebVisualizerModule::DocumentRootChangeCallback(std::string const & docroot)
{
}

// ######################################################################
void *WebVisualizerModule::HTTPRequestCallback(enum mg_event event,
    struct mg_connection *conn,
    const struct mg_request_info *request_info)
{
  if (event == MG_NEW_REQUEST)
  {
    std::string uri(request_info->uri);
    if (uri == "/telemetry.json")
    {
      mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", "{json data}");
      return (void*)""; 
    }
  }
  return NULL;
}

static void *callback(enum mg_event event,
                      struct mg_connection *conn,
                      const struct mg_request_info *request_info) 
{
  return static_cast<WebVisualizerModule*>(request_info->user_data)->HTTPRequestCallback(event, conn, request_info);
}

// ######################################################################
void WebVisualizerModule::preStart()
{
  NRT_INFO("Starting server");
  
  const char *options[] = {
    "listening_ports", nrt::sformat("%d", itsPort.getVal()).c_str(),
    "document_root", itsDocumentRoot.getVal().c_str(),
    NULL };
  
  itsContext = mg_start(&callback, this, options);
  if ( itsContext == NULL )
  {
    NRT_INFO("Failed to start server!");
  }
}

// ######################################################################
void WebVisualizerModule::postStop()
{
  NRT_INFO("Stopping server");
  mg_stop(itsContext);
}

NRT_REGISTER_MODULE(WebVisualizerModule);
