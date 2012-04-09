#include "StereoRectifierModule.H"
#include <nrt/ImageProc/LibraryConversions/OpenCV.H>
#include "StereoCalibrationData.H"

using namespace nrt;
using namespace stereorectifier;

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



  // Clear out the old images
  itsLeftImage  = GenericImage(Image<PixGray<byte>>());
  itsRightImage = GenericImage(Image<PixGray<byte>>());
}

NRT_REGISTER_MODULE(StereoRectifierModule);

