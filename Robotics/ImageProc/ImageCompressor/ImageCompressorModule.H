#ifndef IMAGE_IO_IMAGECOMPRESSOR_IMAGECOMPRESSORMODULE_H
#define IMAGE_IO_IMAGECOMPRESSOR_IMAGECOMPRESSORMODULE_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/ImageProc/IO/ImageSource/ImageSource.H>

#include <Messages/ImageProc/GenericImageMessage.H>
#include <Messages/ImageProc/CompressedImageMessage.H>

namespace imagecompressor
{
  static nrt::ParameterCategory ParamCateg("Image Compressor Related Options");

  static nrt::ParameterDef<int> QualityParamDef("quality", "The compression quality (from 0 to 100)", 75, ParamCateg);

  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(Input, GenericImageMessage,    void, "Input Uncompressed Frames");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Output,    CompressedImageMessage, void, "Output Compressed Frames");
}

//! Compresses Images using JPEG compression
/*! 
    \author Randolph Voorhies

    \keywords image, compression, compress, jpg, jpeg */
class ImageCompressorModule: public nrt::Module,
                             public nrt::MessageSubscriber<imagecompressor::Input>,
                             public nrt::MessagePoster<imagecompressor::Output>
{
  public:
    ImageCompressorModule(std::string const& instanceName = "");

    void onMessage(imagecompressor::Input);
    
  private:
    nrt::Parameter<int> itsQualityParam;
};

#endif // IMAGE_IO_IMAGECOMPRESSOR_IMAGECOMPRESSORMODULE_H

