#ifndef NRT_MODULES_ROBOTICS_SENSING_IMAGEPROC_STEREODEPTH_STEREODEPTHMODULE_H
#define NRT_MODULES_ROBOTICS_SENSING_IMAGEPROC_STEREODEPTH_STEREODEPTHMODULE_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/ImageProc/LibraryConversions/OpenCV.H>
#include <Messages/ImageProc/GenericImageMessage.H>
#include <Messages/ImageProc/StereoPairMessage.H>

namespace stereodepth 
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(StereoPair, StereoPairMessage, void, "The left and right rectified images" );

  static const nrt::ParameterCategory ParamCat("Stereo Depth Related Options");

  static const nrt::ParameterDef<int> sadWindowSizeParamDef("SADWindowSize",
      "The matched block size. Must be an odd number >=1 . Normally, it should be somewhere in 3..11 range", 11, ParamCat);

  static const nrt::ParameterDef<int> minDisparityParamDef("mindisparity",
      "The minimum possible disparity value. Normally it is 0, but sometimes rectification algorithms can shift images, "
      "so this parameter needs to be adjusted accordingly", 0, ParamCat);

  static const nrt::ParameterDef<int> maxDisparityParamDef("maxdisparity",
      "Maximum allowed difference (in integer pixel units) in the left-right disparity check. Set it to non-positive value to disable the check.",
      -1, ParamCat);

  static const nrt::ParameterDef<int> p1ParamDef("p1",
      "Parameter that controls disparity smoothness. The larger the value, the smoother the disparity."
      "P1 is the penalty on the disparity change by plus or minus 1 between neighbor pixels. (p1 < p2)", 2904, ParamCat);

  static const nrt::ParameterDef<int> p2ParamDef("p2",
      "Parameter that controls disparity smoothness. The larger the value, the smoother the disparity."
      "P2 is the penalty on the disparity change by more than 1 between neighbor pixels. (p1 < p2)", 11616, ParamCat);

  static const nrt::ParameterDef<int> uniquenessRatioParamDef("uniquenessratio",
      "The margin in percents by which the best (minimum) computed cost function value should “win” "
      "the second best value to consider the found match correct. Normally, some value within 5-15 range is good enough",
      15, ParamCat);

  static const nrt::ParameterDef<int> speckleWindowSizeParamDef("specklewindowsize",
      "Maximum size of smooth disparity regions to consider them noise speckles and invdalidate. "
      "Set it to 0 to disable speckle filtering. Otherwise, set it somewhere in 50-200 range.",
      200, ParamCat);

  static const nrt::ParameterDef<int> speckleRangeParamDef("specklerange",
      "Maximum disparity variation within each connected component. "
      "If you do speckle filtering, set it to some positive value, multiple of 16. Normally, 16 or 32 is good enough.",
      32, ParamCat);

  static const nrt::ParameterDef<bool> fullDpParamDef("fulldp",
      "Set it to true to run full-scale 2-pass dynamic programming algorithm. "
      "It will consume O(W*H*numDisparities) bytes, which is large for 640x480 stereo and huge for HD-size pictures.",
      true, ParamCat);
}

class StereoDepthModule : public nrt::Module,
                                 nrt::MessageSubscriber<stereodepth::StereoPair>
{
  public:
    StereoDepthModule(std::string const& instanceName="");
    void onMessage(stereodepth::StereoPair msg);

  private:
    std::mutex itsMtx;

    cv::StereoSGBM sgbm;

    nrt::Parameter<int> itsSadWindowSizeParam;
    nrt::Parameter<int> itsP1Param;
    nrt::Parameter<int> itsP2Param;
    nrt::Parameter<int> itsMinDisparityParam;
    nrt::Parameter<int> itsMaxDisparityParam;
    nrt::Parameter<int> itsUniquenessRatioParam;
    nrt::Parameter<int> itsSpeckleWindowSizeParam;
    nrt::Parameter<int> itsSpeckleRangeParam;
    nrt::Parameter<bool> itsFullDpParam;
};

#endif // NRT_MODULES_ROBOTICS_SENSING_IMAGEPROC_STEREODEPTH_STEREODEPTHMODULE_H 
