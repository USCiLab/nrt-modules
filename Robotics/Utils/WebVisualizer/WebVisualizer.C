#include "WebVisualizer.H"
#include "mongoose/mongoose.h"
#include <nrt/Core/Util/StringUtil.H>
#include <nrt/ImageProc/IO/ImageSink/ImageWriters/JpgImageWriter.H>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace nrt;
using namespace webvisualizer;

// ######################################################################
WebVisualizerModule::WebVisualizerModule(std::string const & instanceName) :
  Module(instanceName),
  itsPort(PortParam, this, &WebVisualizerModule::PortChangeCallback),
  itsDocumentRoot(DocumentRootParam, this, &WebVisualizerModule::DocumentRootChangeCallback),
  itsQualityParam(QualtiyParam, this),
  itsVoltage(0), 
  itsCompass(0), 
  itsGyro(0), 
  itsLatitude(34.02061344409650),
  itsLongitude(-118.28542288029116)
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
      // check for messages, update
      if (auto batteryResult = check<webvisualizer::BatteryInfo>(nrt::MessageCheckerPolicy::Unseen))
        itsVoltage = batteryResult.get()->value();

      if (auto gyroResult = check<webvisualizer::GyroInfo>(nrt::MessageCheckerPolicy::Unseen))
        itsGyro = gyroResult.get()->value();

      if (auto compassResult = check<webvisualizer::CompassInfo>(nrt::MessageCheckerPolicy::Unseen))
        itsCompass = compassResult.get()->value();

      if (auto gpsResult = check<webvisualizer::GpsLatLong>(nrt::MessageCheckerPolicy::Unseen))
      {
        std::pair<nrt::real, nrt::real> gps = gpsResult.get()->value();
        itsLatitude = gps.first;
        //itsLatitude = (gps->isNorth ? 1 : -1);
        //itsLatitude *= gps->latitude;

        itsLongitude = gps.second; 
        //itsLongitude = (gps->isWest ? -1 : 1);
        //itsLongitude *= gps->longitude;
      }

      // '{"battery": 10.2, "gyro": 34, "compass": 3.4, "gps": [38.02, -128.99]}'
      std::stringstream ss;
      ss << "{\"battery\": " << itsVoltage << ", \"gyro\": " << itsGyro << ", \"compass\": " << itsCompass << ", \"gps\": [" << itsLatitude << ", " << itsLongitude << "]}\r\n";

      mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", ss.str().c_str());
      return (void*)""; 
    }
    else if (uri == "/viewport.jpg")
    {
      if (auto imageResult = check<webvisualizer::ViewPortImage>(nrt::MessageCheckerPolicy::Any))
      {
        std::vector<byte> imgData = nrt::compressJPG(imageResult.get()->img, itsQualityParam.getVal());
        mg_write(conn, &imgData[0], imgData.size());
      }
      return (void*)""; 
    }
    else if (uri == "/occupancy.jpg")
    {
      if (auto imageResult = check<webvisualizer::OccupancyImage>(nrt::MessageCheckerPolicy::Any))
      {
        std::vector<byte> imgData = nrt::compressJPG(imageResult.get()->img, itsQualityParam.getVal());
        mg_write(conn, &imgData[0], imgData.size());
      }
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
