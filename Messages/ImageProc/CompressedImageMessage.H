#ifndef MESSAGES_IMAGEPROC_COMPRESSEDIMAGEMESSAGE_H
#define MESSAGES_IMAGEPROC_COMPRESSEDIMAGEMESSAGE_H

#include <nrt/Core/Blackboard/Message.H>
#include <nrt/Core/Image/GenericImage.H>
#include <nrt/ImageProc/IO/ImageSource/ImageReaders/JpgImageReader.H>
#include <nrt/ImageProc/IO/ImageSink/ImageWriters/JpgImageWriter.H>
#include <boost/serialization/vector.hpp>

NRT_DECLARE_MESSAGE(CompressedImageMessage)
{
  public:
    CompressedImageMessage() { }

    CompressedImageMessage(nrt::GenericImage img, int quality=75);

    virtual ~CompressedImageMessage() { }

    nrt::GenericImage decompress() const;

  public:
    std::vector<nrt::byte> itsData;
    template <class Archive> inline void serialize(Archive& ar, unsigned int const version) { ar & NRT_VAR(itsData); }
};

#include <Messages/ImageProc/details/CompressedImageMessageImpl.H>
#endif // MESSAGES_IMAGEPROC_COMPRESSEDIMAGEMESSAGE_H
