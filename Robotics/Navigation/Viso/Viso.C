#include "Viso.H"
#include <nrt/Core/Image/Image.H>
#include <nrt/Core/Image/PixelTypes.H>
#include <nrt/ImageProc/IO/ImageSource/V4L2ImageSource.H>
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/Text.H>

using namespace nrt;

// ######################################################################
VisoModule::VisoModule(std::string const& instanceName) :
  Module(instanceName)
{ }

// ######################################################################
void VisoModule::onMessage(viso::StereoPair msg)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsCurrMessage = msg;
}

// ######################################################################
void VisoModule::run()
{
  VisualOdometryStereo::parameters visoParams;
  visoParams.calib.f  = 0.0; // focal length in pixels
  visoParams.calib.cu = 0.0; // principle point (u-coord) in pixels
  visoParams.calib.cv = 0.0; // principle point (v-coord) in pixels
  visoParams.base     = 0.0; // baseline in meters

  // Create the visual odometry engine
  std::shared_ptr<VisualOdometryStereo> visualOdom =
    std::make_shared<VisualOdometryStereo>(visoParams);

  // current pose (this matrix transforms a point from the current frame's
  // coords to the first frame's coords)
  itsPose = Matrix::eye(4);

  while(running())
  {
    nrt::Image<PixGray<byte>> leftImg, rightImg;
    {
      std::lock_guard<std::mutex> _(itsMtx);
      if(itsCurrMessage)
      {
        leftImg  = itsCurrMessage->left.convertTo<PixGray<byte>>();
        rightImg = itsCurrMessage->right.convertTo<PixGray<byte>>();
        itsCurrMessage.reset();
      }
    }

    if(leftImg.size() && rightImg.size())
    {
      int dims[] = { leftImg.width(), leftImg.height(), leftImg.width() };
      if (visualOdom->process(leftImg.pod_begin(), rightImg.pod_begin(), dims))
      {
        itsPose = itsPose * Matrix::inv(visualOdom->getMotion());
      }
      else
      {
        NRT_WARNING("Visual odometry processing failed!");
      }
    }
  }
}

NRT_REGISTER_MODULE(VisoModule);
