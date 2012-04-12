#ifndef NRT_MODULES_MESSAGES_IMAGEPROC_STEREOPAIRMESSAGE_H
#define NRT_MODULES_MESSAGES_IMAGEPROC_STEREOPAIRMESSAGE_H

#include <nrt/Eigen/Eigen.H>
#include <nrt/Core/Blackboard/Message.H>
#include <nrt/Core/Image/GenericImage.H>

//! A calibrated and rectified stereo pair of images
NRT_DECLARE_MESSAGE(StereoPairMessage)
{
  public:
    StereoPairMessage() { }
    StereoPairMessage(nrt::GenericImage left_, nrt::GenericImage right_) : 
      left(left_),
      right(right_) { }

    virtual ~StereoPairMessage() { }

    // ! The left image
    nrt::GenericImage left;

    //! The right image
    nrt::GenericImage right;

    //! A 4x4 disparity-to-depth mapping matrix
    Eigen::Matrix4f Q;

    template<class Archive> inline void serialize(Archive& ar, unsigned int const version)
    {
      ar & NRT_VAR(left);
      ar & NRT_VAR(right);
      ar & NRT_VAR(Q);
    }
};

#endif // NRT_MODULES_MESSAGES_IMAGEPROC_STEREOPAIRMESSAGE_H
