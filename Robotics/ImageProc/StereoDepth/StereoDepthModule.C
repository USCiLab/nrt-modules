#include "StereoDepthModule.H"

using namespace nrt;
using namespace stereodepth;

// ######################################################################
StereoDepthModule::StereoDepthModule(std::string const& instanceName) :
  Module(instanceName)
{ }

// ######################################################################
void StereoDepthModule::onMessage(stereodepth::StereoPair msg)
{
  std::lock_guard<std::mutex> _(itsMtx);

  if(msg->left.size() != msg->right.size())
    throw exception::ModuleException(this, __PRETTY_FUNCTION__, "Left and right images are different sizes!");

  sgbm.SADWindowSize       = itsSadWindowSizeParam.getVal();
  sgbm.P1                  = itsP1Param.getVal()
  sgbm.P2                  = itsP2Param.getVal()
  sgbm.disp12MaxDiff       = itsMaxDisparityParam.getVal();
  sgbm.minDisparity        = itsMinDisparityParam.getVal();
  sgbm.numberOfDisparities = ((msg->left.width()/8) + 15) & -16;
  sgbm.uniquenessRatio     = itsUniquenessRatioParam.getVal();
  sgbm.speckleWindowSize   = itsSpeckleWindowSizeParam.getVal();
  sgbm.speckleRange        = itsSpeckleRangeParam.getVal();
  sgbm.fullDP              = itsFullDpParam.getVal();

  cv::Mat leftmat  = copyImage2CvMat(msg->left.convertTo<PixRGB<byte>>());
  cv::Mat rightmat = copyImage2CvMat(msg->right.convertTo<PixRGB<byte>>());

  cv::Mat disparity;
  sgbm(leftRemapped, rightRemapped, disparity); 

}

NRT_REGISTER_MODULE(StereoDepthModule);
