#ifndef NRT_MODULES_MESSAGES_IMAGEPROC_STEREOPAIRMESSAGE_H
#define NRT_MODULES_MESSAGES_IMAGEPROC_STEREOPAIRMESSAGE_H

#include <nrt/Core/Blackboard/Message.H>
#include <nrt/Core/Image/GenericImage.H>

NRT_DECLARE_MESSAGE(StereoPairMessage)
{
  public:
    StereoPairMessage() { }
    StereoPairMessage(nrt::GenericImage left_, nrt::GenericImage right_) : 
      left(left_),
      right(right_) { }
    virtual ~StereoPairMessage() { }

    nrt::GenericImage left;
    nrt::GenericImage right;

    template<class Archive> inline void serialize(Archive& ar, unsigned int const version)
    {
      ar & NRT_VAR(left);
      ar & NRT_VAR(right);
    }
};

#endif // NRT_MODULES_MESSAGES_IMAGEPROC_STEREOPAIRMESSAGE_H
