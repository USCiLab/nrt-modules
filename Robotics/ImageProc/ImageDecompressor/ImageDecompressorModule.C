#include "ImageDecompressorModule.H"

using namespace imagedecompressor;

// ######################################################################
ImageDecompressorModule::ImageDecompressorModule(std::string const& instanceName) :
  Module(instanceName)
{ }

// ######################################################################
void ImageDecompressorModule::onMessage(Input inMsg)
{
  GenericImageMessage::unique_ptr outMsg(new GenericImageMessage(inMsg->decompress()));
  post<Output>(outMsg);
}

NRT_REGISTER_MODULE(ImageDecompressorModule);
