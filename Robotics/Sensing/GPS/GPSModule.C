#include "GPSModule.H"
#include <SerialPort.h>
#include <iomanip>

using namespace nrt;

// ######################################################################
GPSModule::GPSModule(std::string const & instanceName) :
  Module(instanceName),
  itsSerialPort(nullptr),
  itsGpsDev(gps::GPSDevParam, this, &GPSModule::gpsDevCallback),
   itsOriginLat(gps::GPSOriginLatParam, this, nullptr),
   itsOriginLng(gps::GPSOriginLngParam, this, nullptr),
   itsFromParam(gps::GpsFrameParam, this, nullptr),
  itsFrameParam(gps::FromFrameParam, this, nullptr)
{
  //
}

// ######################################################################
void GPSModule::gpsDevCallback(std::string const & dev)
{
  std::lock_guard<std::mutex> _(itsMtx);

  if (itsSerialPort) itsSerialPort.reset();
  if (dev == "") return;

  try
  {
    itsSerialPort = std::make_shared<SerialPort>(dev);
    itsSerialPort->Open(
        SerialPort::BAUD_115200,
        SerialPort::CHAR_SIZE_8,
        SerialPort::PARITY_NONE,
        SerialPort::STOP_BITS_1,
        SerialPort::FLOW_CONTROL_NONE);
  }
  catch(SerialPort::AlreadyOpen & e)
  {
    // If it's already open, then who cares...
  }
  catch(SerialPort::OpenFailed & e)
  {
    throw nrt::exception::BadParameter(std::string("Open Failed: ") + e.what());
  }
  catch(std::invalid_argument & e)
  {
    throw nrt::exception::BadParameter(std::string("Invalid Argument: ") + e.what());
  }

  if(!itsSerialPort->IsOpen())
    throw nrt::exception::BadParameter("Failed to open serial port");
}

void GPSModule::run()
{
  while(running())
  {
    bool paragraphComplete = false;
    nrt::GPSMessage::unique_ptr msg(new nrt::GPSMessage);

    if (!itsSerialPort)
    {
      sleep(1);
      continue;
    }

    while (!paragraphComplete)
    {
      try
      {
        std::string line = itsSerialPort->ReadLine(1000);
        std::vector<std::string> fields = nrt::splitString(line, ',');

        if (fields[0] == "$GPGGA")
        {
          // $GPGGA,053231.000,3401.2363,N,11817.2715,W,2,7,1.36,130.6,M,-33.8,M,0000,0000*64

          // "fix" data
          msg->timestamp     = boost::lexical_cast<double>(fields[1]);
          msg->latitude      = std::atof(fields[2].c_str());//boost::lexical_cast<double>(fields[2]);
          msg->isNorth       = (fields[3] == "N");
          msg->longitude     = std::atof(fields[4].c_str());//boost::lexical_cast<double>(fields[4]);
          msg->isWest        = (fields[5] == "W");
          msg->quality       = boost::lexical_cast<int>(fields[6]);
          msg->numSatellites = boost::lexical_cast<int>(fields[7]);
          msg->hdop          = boost::lexical_cast<double>(fields[8]);
          msg->altitude      = boost::lexical_cast<double>(fields[9]);
          msg->geoid         = boost::lexical_cast<double>(fields[11]);
          msg->lastDGPStime  = boost::lexical_cast<double>(fields[13]);
          msg->dgpsId        = boost::lexical_cast<int>(fields[14]);

          if (msg->quality == 0)
          {
            NRT_INFO("Invalid GPS fix!");
            continue;
          }
        }
        else if (fields[0] == "$GPGSA")
        {
          // $GPGSA,A,3,17,28,07,08,26,15,27,11,,,,,2.02,1.31,1.53*00
          // degrees of precision and active satellites
          msg->isAuto  = (fields[1] == "A");
          msg->fixMode = nrt::GPSMessage::FixModes(boost::lexical_cast<int>(fields[2]));
          msg->dop     = boost::lexical_cast<double>(fields[15]);
          msg->hdop    = boost::lexical_cast<double>(fields[16]);
          msg->vdop    = boost::lexical_cast<double>(fields[17]);

          if (msg->fixMode == nrt::GPSMessage::FixModes::NoFix)
          {
            NRT_INFO("No GPS fix! Move to a clearer area.");
            continue;
          }
        }
        else if (fields[0] == "$GPRMC")
        {
          // $GPRMC,053658.000,A,3401.2371,N,11817.2822,W,0.01,60.63,100312,,,D*42
          // recommended minimum data
          msg->isActive   = (fields[2] == "A");
          msg->speed      = boost::lexical_cast<double>(fields[7]);
          msg->trackAngle = boost::lexical_cast<double>(fields[8]); // angle the thing is travelling at (not pointing)
          msg->date       = boost::lexical_cast<int>(fields[9]);

          if (!msg->isActive)
          {
            NRT_INFO("Invalid GPS data!");
            continue;
          }

          double deg = int(msg->latitude/100);
          double min = 100*((msg->latitude/100)-deg);
          double lat = (deg + min/60) * (msg->isNorth ? 1 : -1);

          deg = int(msg->longitude/100);
          min = 100*((msg->longitude/100)-deg);
          double lon = (deg + min/60) * (msg->isWest ? -1 : 1);

          // convert GPS to X,Y,Z
          double x = gpsDistance(itsOriginLat.getVal(), itsOriginLng.getVal(), lat, itsOriginLng.getVal());
          double y = gpsDistance(itsOriginLat.getVal(), itsOriginLng.getVal(), itsOriginLat.getVal(), lon);
          double z = 0;

          std::cout << std::setprecision(16);

          // post the transform
          NRT_INFO("Posting transform with distance from (" << itsOriginLat.getVal() << ", " << itsOriginLng.getVal() <<
                   ") to (" << lat << ", " << lon << ") = (" << x << ", " << y << ") = " << sqrt(x*x+y*y) << " meters");
          itsTransform = Eigen::Translation3d(x, y, z);
          std::unique_ptr<nrt::TransformMessage> transformMsg(new nrt::TransformMessage(nrt::now(), itsFromParam.getVal(), itsFrameParam.getVal(), itsTransform));
          post<gps::GpsTransform>(transformMsg);

          // post the GPS message
          NRT_INFO("Posting GPS message: timestamp " << msg->timestamp << " " << msg->latitude/100 << "N, " << msg->longitude/100 << "W, altitude " << msg->altitude << "m from " << msg->numSatellites << " satellites.");
          post<gps::GpsMessage>(msg);
          
          paragraphComplete = true;
        }
      }
      catch (SerialPort::ReadTimeout & e)
      {
        usleep(1000000);
      }
      catch(boost::bad_lexical_cast const & e)
      {
        continue;
      }
    }
  }
  itsSerialPort->Close();
}

double GPSModule::gpsDistance(double lat1, double lon1, double lat2, double lon2)
{
  double R = 6371000; // earth radius in meters
  double dLat = (lat2-lat1) * M_PI/180;
  double dLon = (lon2-lon1) * M_PI/180;

  double rlat1 = lat1 * M_PI/180;
  double rlat2 = lat2 * M_PI/180;

  double a = sin(dLat/2) * sin(dLat/2) +
             sin(dLon/2) * sin(dLon/2) * cos(rlat1) * cos(rlat2); 
  double c = 2 * atan2(sqrt(a), sqrt(1-a)); 
  double d = R * c;

  return d;
}


NRT_REGISTER_MODULE(GPSModule);
