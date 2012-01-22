#ifndef WEBVISUALIZER_H 
#define WEBVISUALIZER_H 

#include "mongoose/mongoose.h"
#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>

namespace webvisualizer 
{
  NRT_DECLARE_MESSAGEPOSTER_PORT( VelocityCommand, nrt::VelocityMessage, void, "A velocity to send" );

  static const nrt::ParameterCategory ParamCat("WebVisualizer Related Options");

  static const nrt::ParameterDef<int> PortParam("port", "The port to listen on", 9000, ParamCat);

  static const nrt::ParameterDef<std::string> DocumentRootParam("docroot", "The directory from whence to serve documents", "/home/sagar/workspace/nrt-modules/Robotics/Utils/WebVisualizer/html/", ParamCat);
}

//! 
/*!  */
class WebVisualizerModule: public nrt::Module
{
  public:
    WebVisualizerModule(std::string const& instanceName="");

    void PortChangeCallback(int const & port);

    void DocumentRootChangeCallback(std::string const & docroot);

    void *HTTPRequestCallback(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *request_info);
    
    // Start up our webserver
    void preStart();
    
    // Close down that webserver
    void postStop();

  private:
    std::mutex itsMtx;
    nrt::Parameter<int> itsPort;
    nrt::Parameter<std::string> itsDocumentRoot;
    struct mg_context *itsContext;
};

#endif // WEBVISUALIZER_H 
