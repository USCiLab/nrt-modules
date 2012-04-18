#include "ImageCompressorModule.H"

using namespace imagecompressor;

// ######################################################################
ImageCompressorModule::ImageCompressorModule(std::string const& instanceName) :
  Module(instanceName),
  itsQualityParam(QualityParamDef, this)
{ }

// ######################################################################
void ImageCompressorModule::onMessage(Input inMsg)
{
  CompressedImageMessage::unique_ptr outMsg(new CompressedImageMessage(inMsg->img, itsQualityParam.getVal()));
  post<Output>(outMsg);
}

NRT_REGISTER_MODULE(ImageCompressorModule);
