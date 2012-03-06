#ifndef NRT_ROBOTICS_MESSAGES_JOYSTICKMESSAGE_H_
#define NRT_ROBOTICS_MESSAGES_JOYSTICKMESSAGE_H_

#include <nrt/Eigen/Types.H>
#include <nrt/Core/Blackboard/Message.H>
#include <boost/serialization/vector.hpp>

namespace nrt
{
  //! A message to encapsulate joystick data
  /*! This message wraps joystick commands in a generic way that makes them available to other modules. It's up to the
    other module (for example, a teleop module) to translate these JoystickMessages into velocity messages or something
    else that's meaningful. */
  
  NRT_DECLARE_MESSAGE( JoystickMessage )
  {
  public:
    std::vector<int> axes;
    std::vector<int> buttons;
    
    template<class Archive>
    inline void serialize( Archive& ar, const unsigned int version )
    {
      ar & axes;
      ar & buttons;
    }
  };

}

#endif // NRT_ROBOTICS_MESSAGES_JOYSTICKMESSAGE_H_
