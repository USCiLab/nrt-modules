#ifndef IMAGE_IO_IMAGEDECOMPRESSOR_IMAGEDECOMPRESSORMODULE_H
#define IMAGE_IO_IMAGEDECOMPRESSOR_IMAGEDECOMPRESSORMODULE_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/ImageProc/IO/ImageSource/ImageSource.H>

#include <Messages/ImageProc/GenericImageMessage.H>
#include <Messages/ImageProc/CompressedImageMessage.H>

namespace imagedecompressor
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(Input, CompressedImageMessage, void, "Input Compressed Frames");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Output,    GenericImageMessage,    void, "Output Decompressed Frames");
}

//! Decompresses Images using JPEG compression
/*! 
    \author Randolph Voorhies

    \keywords image, compression, compress, jpg, jpeg */
class ImageDecompressorModule: public nrt::Module,
                               public nrt::MessageSubscriber<imagedecompressor::Input>,
                               public nrt::MessagePoster<imagedecompressor::Output>
{
  public:
    ImageDecompressorModule(std::string const& instanceName = "");

    void onMessage(imagedecompressor::Input);
};

#endif // IMAGE_IO_IMAGEDECOMPRESSOR_IMAGEDECOMPRESSORMODULE_H

