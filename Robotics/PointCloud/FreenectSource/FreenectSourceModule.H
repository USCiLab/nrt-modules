/*! @file PointCloud/FreenectSource/FreenectSourceModule.H */

#ifndef POINTCLOUD_FREENECTSOURCE_FREENECTSOURCEMODULE_H
#define POINTCLOUD_FREENECTSOURCE_FREENECTSOURCEMODULE_H

#include <nrt/Core/Blackboard/Module.H>
#include <Messages/PointCloud/GenericCloudMessage.H>
#include <Messages/ImageProc/GenericImageMessage.H>
#include <libfreenect/libfreenect.h>
#include <libfreenect/libfreenect-registration.h>

// namespace for ports and parameters of FreenectSourceModule
namespace freenectsourcemodule
{
  NRT_DECLARE_MESSAGEPOSTER_PORT(RGBImage,   GenericImageMessage, void, "Color image from the RGB sensor");
  NRT_DECLARE_MESSAGEPOSTER_PORT(DepthImage, GenericImageMessage, void, "Depth image from the IR sensor");
	NRT_DECLARE_MESSAGEPOSTER_PORT(Cloud,      GenericCloudMessage, void, "The output point cloud generated from the depth image");
}

//! Publishes data from a Kinect using the libfreenect library
/*! 
    @author Randolph Voorhies
    @keywords kinect, point cloud, pointcloud, rgbd */
class FreenectSourceModule : public nrt::Module,
               public nrt::MessagePoster<freenectsourcemodule::RGBImage,
                                         freenectsourcemodule::DepthImage,
                                         freenectsourcemodule::Cloud>
{
  public:
    //! Constructor
    FreenectSourceModule(std::string const & instanceName = "");

    //! Run method
    void run();

    //! Virtual destructor for safe inheritance
    virtual ~FreenectSourceModule();

   void depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp);
   void depthHandlerThread();

   void rgbCallback(freenect_device *dev, void *rgb, uint32_t timestamp);
   void rgbHandlerThread();

   std::mutex itsDepthMtx;
   std::condition_variable itsDepthCondition;
   std::vector<uint16_t> itsFrontDepthBuffer;
   std::vector<uint16_t> itsBackDepthBuffer;

   std::mutex itsRGBMtx;
   std::mutex itsRGBImageMtx;
   std::condition_variable itsRGBCondition;
   std::vector<uint8_t> itsFrontRGBBuffer;
   std::vector<uint8_t> itsMidRGBBuffer;
   std::vector<uint8_t> itsBackRGBBuffer;
   nrt::Image<nrt::PixRGB<uint8_t>> itsRGBImage;

   freenect_device * itsDev;
   freenect_registration itsRegistration;
};

#endif // POINTCLOUD_FREENECTSOURCE_FREENECTSOURCEMODULE_H
