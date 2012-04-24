CompressedImageMessage::CompressedImageMessage(nrt::GenericImage img, int quality)
{
  itsData = nrt::compressJPG(img, quality);

}

nrt::GenericImage CompressedImageMessage::decompress() const
{
  return nrt::decompressJPG(itsData);
}

