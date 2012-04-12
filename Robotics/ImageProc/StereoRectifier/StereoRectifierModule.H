#ifndef NRT_MODULES_ROBOTICS_SENSING_IMAGEPROC_STEREORECTIFIER_STEREORECTIFIERMODULE_H
#define NRT_MODULES_ROBOTICS_SENSING_IMAGEPROC_STEREORECTIFIER_STEREORECTIFIERMODULE_H 

#include <nrt/Core/Blackboard/Module.H>
#include <Messages/ImageProc/GenericImageMessage.H>
#include <Messages/ImageProc/StereoPairMessage.H>

class StereoCalibrationData;

namespace stereorectifier 
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(LeftImage,  GenericImageMessage, void, "The left image" );
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(RightImage, GenericImageMessage, void, "The right image" );
  NRT_DECLARE_MESSAGEPOSTER_PORT(StereoPair, StereoPairMessage, void, "The left and right rectified images" );

  static const nrt::ParameterCategory ParamCat("Stereo Rectifier Related Options");
  static const nrt::ParameterDef<std::string> calibrationFileParamDef("calibrationfile", "The calibration file location", "", ParamCat);
}

//! Rectify a left and right stereo pair, combining them into a single StereoPairMessage
/*! This module provides rectifies and calibrates two independent camera images so that they may be used, e.g. to measure distance.
    To operate, this module requires a calibration file created by the calibrateCameras application included with this module.

    \author Randolph Voorhies
    \keywords stereo, calibration, image, image processing, depth, distance */
class StereoRectifierModule : public nrt::Module,
                                     nrt::MessageSubscriber<stereorectifier::LeftImage,
                                                            stereorectifier::RightImage>,
                                     nrt::MessagePoster<stereorectifier::StereoPair>
{
  public:
    StereoRectifierModule(std::string const& instanceName="");
    void onMessage(stereorectifier::LeftImage msg);
    void onMessage(stereorectifier::RightImage msg);

  private:
    void process();

    nrt::GenericImage itsLeftImage;
    nrt::GenericImage itsRightImage;

    class RMaps;
    std::shared_ptr<StereoCalibrationData> itsCalibrationData;
    std::shared_ptr<RMaps> itsRMaps;

    void calibrationFileCallback(std::string const& filename);
    std::mutex itsMtx;
    nrt::Parameter<std::string> itsCalibrationFileParam;
};

#endif // NRT_MODULES_ROBOTICS_SENSING_IMAGEPROC_STEREORECTIFIER_STEREORECTIFIERMODULE_H 
