#ifndef NRT_ROBOTICS_MESSAGES_GPSMESSAGE_H_
#define NRT_ROBOTICS_MESSAGES_GPSMESSAGE_H_

#include <nrt/Eigen/Types.H>
#include <nrt/Core/Blackboard/Message.H>
#include <boost/serialization/vector.hpp>

namespace nrt
{
  //! A message to encapsulate joystick data
  /*! This message wraps joystick commands in a generic way that makes them available to other modules. It's up to the
    other module (for example, a teleop module) to translate these JoystickMessages into velocity messages or something
    else that's meaningful. */

  NRT_DECLARE_MESSAGE( GPSMessage )
  {
    public:
      enum FixModes
      { 
        NoFix  = 1,
        TwoD   = 2,
        ThreeD = 3
      };

      bool isActive;
      bool isAuto;
      bool isNorth;
      bool isWest;

      FixModes fixMode;

      int date;
      int dgpsId;
      int numSatellites;
      int quality;

      double altitude;
      double dop;
      double geoid;
      double hdop;
      double lastDGPStime;
      double latitude;
      double longitude;
      double speed;
      double timestamp;
      double trackAngle;
      double vdop;
    
    template<class Archive>
    inline void serialize( Archive& ar, const unsigned int version )
    {
      ar & isActive;
      ar & isAuto;
      ar & isNorth;
      ar & isWest;

      ar & date;
      ar & dgpsId;
      ar & fixMode;
      ar & numSatellites;
      ar & quality;

      ar & altitude;
      ar & dop;
      ar & geoid;
      ar & hdop;
      ar & hdop;
      ar & lastDGPStime;
      ar & latitude;
      ar & longitude;
      ar & speed;
      ar & timestamp;
      ar & trackAngle;
      ar & vdop;
    }
  };

}

#endif // NRT_ROBOTICS_MESSAGES_GPSMESSAGE_H_
