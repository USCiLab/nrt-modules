#ifndef WEBVISUALIZER_H 
#define WEBVISUALIZER_H 

#include "mongoose/mongoose.h"
#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>
#include <Messages/Sensors/GPSMessage.H>
#include <Messages/ImageProc/GenericImageMessage.H>

namespace webvisualizer 
{
  NRT_DECLARE_MESSAGECHECKER_PORT(BatteryInfo, nrt::Message<nrt::real>, "Hermes battery data.");
  NRT_DECLARE_MESSAGECHECKER_PORT(CompassInfo, nrt::Message<nrt::real>, "Hermes compass data.");
  NRT_DECLARE_MESSAGECHECKER_PORT(GyroInfo, nrt::Message<nrt::real>, "Hermes gyroscope data.");
  NRT_DECLARE_MESSAGECHECKER_PORT(GPSInfo, nrt::GPSMessage, "Hermes GPS data.");
  NRT_DECLARE_MESSAGECHECKER_PORT(ViewPortImage, GenericImageMessage, "An image to show in the background of the HUD");
  NRT_DECLARE_MESSAGECHECKER_PORT(OccupancyImage, GenericImageMessage, "The occupancy grid.");

  static const nrt::ParameterCategory ParamCat("WebVisualizer Related Options");
  static const nrt::ParameterDef<int> PortParam("port", "The port to listen on", 9000, ParamCat);
  static const nrt::ParameterDef<std::string> DocumentRootParam("docroot", "The directory from whence to serve documents", "/home/sagar/workspace/nrt-modules/Robotics/Utils/WebVisualizer/html/", ParamCat);
  static const nrt::ParameterDef<int> QualtiyParam("quality", "The quality of the image that's published.", 50, ParamCat);
}

class WebVisualizerModule: public nrt::Module,
  public nrt::MessageChecker<webvisualizer::BatteryInfo,
    webvisualizer::CompassInfo,
    webvisualizer::GyroInfo,
    webvisualizer::GPSInfo,
    webvisualizer::ViewPortImage,
    webvisualizer::OccupancyImage>
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
    nrt::Parameter<int> itsQualityParam;
    struct mg_context *itsContext;
    double itsVoltage, itsCompass, itsGyro, itsLatitude, itsLongitude;
};

#endif // WEBVISUALIZER_H 
