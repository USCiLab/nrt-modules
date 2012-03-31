#ifndef GPSMODULE_H
#define GPSMODULE_H

#include <nrt/Core/Blackboard/Module.H>
#include <Messages/Sensors/GPSMessage.H>
#include <nrt/Robotics/Messages/TransformMessage.H>

class SerialPort;

namespace gps
{
  NRT_DECLARE_MESSAGEPOSTER_PORT(GpsMessage, nrt::GPSMessage, void, "The GPS data.");
  NRT_DECLARE_MESSAGEPOSTER_PORT(GpsTransform, nrt::TransformMessage, void, "The GPS dongle's transform.");
  
  nrt::ParameterCategory ParamCat("GPS device related options");
  nrt::ParameterDef<std::string> GPSDevParam("gpsdev", "The GPS device file", "/dev/ttyACM0", ParamCat);
  nrt::ParameterDef<nrt::real> GPSOriginLatParam("originlat", "The latitude of the origin of this GPS module's world", 34.020613444096504, ParamCat);
  nrt::ParameterDef<nrt::real> GPSOriginLngParam("originlng", "The longitude of the origin of this GPS module's world", -118.28542288029116, ParamCat);
  nrt::ParameterDef<std::string> GpsFrameParam("frame", "The name of the GPS frame", "gps", ParamCat);
  nrt::ParameterDef<std::string> FromFrameParam("from", "The name of the parent frame of the GPS", "world", ParamCat);
}

class GPSModule : public nrt::Module, public nrt::MessagePoster<gps::GpsMessage, gps::GpsTransform>
{
  public:
    GPSModule(std::string const & instanceName="");

    void run();

    void gpsDevCallback(std::string const & dev);

    double gpsDistance(double lat1, double lng1, double lat2, double lng2);

  private:
    std::mutex itsMtx;
    std::shared_ptr<SerialPort> itsSerialPort;
    nrt::Parameter<std::string> itsGpsDev;
    nrt::Parameter<nrt::real> itsOriginLat;
    nrt::Parameter<nrt::real> itsOriginLng;
    nrt::Parameter<std::string> itsFromParam;
    nrt::Parameter<std::string> itsFrameParam;
    nrt::Transform3D itsTransform;

};

#endif // GPSMODULE_H
