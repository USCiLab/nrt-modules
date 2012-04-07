#include "StereoRectifierModule.H"
#include <nrt/ImageProc/LibraryConversions/OpenCV.H>
#include "StereoCalibrationData.H"
//#include <opencv2/core/eigen.hpp>

using namespace nrt;
using namespace stereorectifier;

struct StereoRectifierModule::RMaps { cv::Mat map[2][2]; };

// ######################################################################
StereoRectifierModule::StereoRectifierModule(std::string const& instanceName) :
  Module(instanceName),
  itsCalibrationFileParam(calibrationFileParamDef, this, &StereoRectifierModule::calibrationFileCallback)
{ }

// ######################################################################
void StereoRectifierModule::calibrationFileCallback(std::string const& filename)
{
  if(filename == "") return;

  try { itsCalibrationData = std::make_shared<StereoCalibrationData>(filename); }
  catch(...) { throw exception::BadParameter("Could not open calibration file"); }

  itsRMaps.reset();
}

// ######################################################################
void StereoRectifierModule::onMessage(LeftImage msg)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsLeftImage = msg->img;
  process();
}

// ######################################################################
void StereoRectifierModule::onMessage(RightImage msg)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsRightImage = msg->img;
  process();
}

// ######################################################################
void StereoRectifierModule::process()
{
  if(itsLeftImage.size() == 0 || itsRightImage.size() == 0) return;
  if(itsLeftImage.size() != itsRightImage.size())
    throw exception::ModuleException(this, __PRETTY_FUNCTION__, "Left and right images are different sizes!");

  // TODO: Run as a visitor to preserve original colorspace
  cv::Mat leftmat  = copyImage2CvMat(Image<PixRGB<byte>>(itsLeftImage.convertTo<PixRGB<byte>>()));
  cv::Mat rightmat = copyImage2CvMat(Image<PixRGB<byte>>(itsRightImage.convertTo<PixRGB<byte>>()));

  if(itsRMaps)
  {
    itsRMaps = std::make_shared<RMaps>();
    cv::Size imageSize = leftmat.size();
    initUndistortRectifyMap(itsCalibrationData->leftIntrinsics, itsCalibrationData->leftDistortion,
        itsCalibrationData->R, itsCalibrationData->P1, imageSize, CV_16SC2, itsRMaps->map[0][0], itsRMaps->map[0][1]);
    initUndistortRectifyMap(itsCalibrationData->rightIntrinsics, itsCalibrationData->rightDistortion,
        itsCalibrationData->R, itsCalibrationData->P1, imageSize, CV_16SC2, itsRMaps->map[1][0], itsRMaps->map[1][1]);
  }

  cv::Mat leftRemapped, rightRemapped;
  cv::remap(leftmat,  leftRemapped,  itsRMaps->map[0][0], itsRMaps->map[0][1], CV_INTER_LINEAR);
  cv::remap(rightmat, rightRemapped, itsRMaps->map[1][0], itsRMaps->map[1][1], CV_INTER_LINEAR);

  StereoPairMessage::unique_ptr stereomsg(new StereoPairMessage);
  stereomsg->left  = GenericImage(copyCvMat2Image<PixRGB<byte>>(leftRemapped));
  stereomsg->right = GenericImage(copyCvMat2Image<PixRGB<byte>>(rightRemapped));
  cv::cv2eigen(itsCalibrationData->Q, stereomsg->Q);
  post<StereoPair>(stereomsg);

  // Clear out the old images
  itsLeftImage  = GenericImage(Image<PixGray<byte>>());
  itsRightImage = GenericImage(Image<PixGray<byte>>());
}

NRT_REGISTER_MODULE(StereoRectifierModule);

