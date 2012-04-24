#ifndef NRT_ROBOTICS_MESSAGES_PATHMESSAGE_H
#define NRT_ROBOTICS_MESSAGES_PATHMESSAGE_H

#include <nrt/Eigen/Types.H>
#include <nrt/Core/Blackboard/Message.H>
#include <boost/serialization/vector.hpp>

namespace nrt
{
  //! A message to encapsulate joystick data
  /*! This message wraps joystick commands in a generic way that makes them available to other modules. It's up to the
    other module (for example, a teleop module) to translate these JoystickMessages into velocity messages or something
    else that's meaningful. */
  
  NRT_DECLARE_MESSAGE( PathMessage )
  {
  public:
    PathMessage() { }
    PathMessage(std::vector<Point2D<int>> path_) : path(path_) { }

    std::vector<Point2D<int>> path;
    
    template<class Archive>
    inline void serialize( Archive& ar, const unsigned int version )
    {
      ar & NRT_VAR(path);
    }
  };

}

#endif // NRT_ROBOTICS_MESSAGES_PATHMESSAGE_H
