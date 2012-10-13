/*! @file PointCloud/FreenectSource/FreenectSourceModule.C */

#include "FreenectSourceModule.H"
#include <libfreenect.hpp>

// ######################################################################
FreenectSourceModule::FreenectSourceModule(std::string const & instanceName) :
    nrt::Module(instanceName),
    itsRGBImage(640, 480),
    itsDepthImage(640, 480)
{  }

// ######################################################################
FreenectSourceModule::~FreenectSourceModule()
{  }

// ######################################################################
void depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
  FreenectSourceModule * module = static_cast<FreenectSourceModule*>(freenect_get_user(dev));
  module->depthCallback(dev, v_depth, timestamp);
}
// ######################################################################
void FreenectSourceModule::depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
  {
    std::unique_lock<std::mutex> _(itsDepthMtx);
    std::memcpy(&itsFrontDepthBuffer[0], v_depth, sizeof(uint16_t) * itsFrontDepthBuffer.size());
  }
  itsDepthCondition.notify_all();
}
// ######################################################################
void FreenectSourceModule::depthHandlerThread()
{
  while(running())
  {
    {
      std::unique_lock<std::mutex> _(itsDepthMtx);
      itsDepthCondition.wait(_);
      std::swap(itsFrontDepthBuffer, itsBackDepthBuffer);
    }
    if(!running()) return;

    nrt::Image<nrt::PixRGB<uint8_t>> rgbImage;
    {
      std::unique_lock<std::mutex> _(itsRGBImageMtx);
      rgbImage = itsRGBImage;
      rgbImage.deepCopy();
    }

    uint16_t const * depth_in_ptr = &itsBackDepthBuffer[0];
    nrt::PixRGB<uint8_t> const * rgb_ptr = rgbImage.const_begin();

    nrt::Image<nrt::PixGray<float>> depthImage(640, 480);
    float * depth_out_ptr = depthImage.pod_begin();

    nrt::PointCloud<nrt::PointXYZRGBAF> cloud(640*480);
    nrt::PointCloud<nrt::PointXYZRGBAF>::iterator cloud_out_ptr = cloud.begin();

    float const ref_pix_size = itsRegistration.zero_plane_info.reference_pixel_size;
    float const ref_distance = itsRegistration.zero_plane_info.reference_distance;
    float const ref_factor = 2.0F * ref_pix_size  / ref_distance;

    float const center_x = 640.0F / 2.0F;
    float const center_y = 480.0F / 2.0F;

    size_t num_points = 0;
    for(int cam_y=0; cam_y<480; ++cam_y)
      for(int cam_x=0; cam_x<640; ++cam_x)
      {
        float const world_z = (*depth_in_ptr) / 100.0F;
        if(world_z != FREENECT_DEPTH_MM_NO_VALUE)
        {
          float const factor = ref_factor * world_z;
          float const world_x = (cam_x - center_x) * factor;
          float const world_y = (cam_y - center_y) * factor;

          (*depth_out_ptr) = (*depth_in_ptr);

          nrt::Point3DEf & pos = cloud_out_ptr->get<nrt::Point3DEf>();
          pos.x() = world_x;
          pos.y() = world_y;
          pos.z() = world_z;

          nrt::PixRGBA<float> & rgb = cloud_out_ptr->get<nrt::PixRGBA<float>>();
          rgb.setR(rgb_ptr->r());
          rgb.setG(rgb_ptr->g());
          rgb.setB(rgb_ptr->b());
          rgb.setA(255.0F);

          ++num_points;
        }

        ++depth_in_ptr;
        ++depth_out_ptr;
        ++rgb_ptr;
        ++cloud_out_ptr;
      }

    cloud.resize(num_points);

    std::unique_ptr<GenericImageMessage> depthmsg(new GenericImageMessage(depthImage));
    post<freenectsourcemodule::DepthImage>(depthmsg);

    std::unique_ptr<GenericCloudMessage> cloudmsg(new GenericCloudMessage(cloud));
    post<freenectsourcemodule::Cloud>(cloudmsg);

    itsDepthImage = depthImage;
  }
}

// ######################################################################
void rgbCallback(freenect_device *dev, void *rgb, uint32_t timestamp)
{
  FreenectSourceModule * module = static_cast<FreenectSourceModule*>(freenect_get_user(dev));
  module->rgbCallback(dev, rgb, timestamp);
}

// ######################################################################
void FreenectSourceModule::rgbCallback(freenect_device *dev, void *rgb, uint32_t timestamp)
{
  {
    std::unique_lock<std::mutex> _(itsRGBMtx);
    std::swap(itsFrontRGBBuffer, itsMidRGBBuffer);
    freenect_set_video_buffer(dev, &itsFrontRGBBuffer[0]);
  }
  itsRGBCondition.notify_all();
}

// ######################################################################
void FreenectSourceModule::rgbHandlerThread()
{
  while(running())
  {
    {
      std::unique_lock<std::mutex> _(itsRGBMtx);
      itsRGBCondition.wait(_);
      std::swap(itsMidRGBBuffer, itsBackRGBBuffer);
    }
    if(!running()) return;

    nrt::Image<nrt::PixRGB<uint8_t>> image(640, 480);
    memcpy(image.pod_begin(), &itsBackRGBBuffer[0], sizeof(char)*640*480*3);

    {
      std::unique_lock<std::mutex> _(itsRGBImageMtx);
      itsRGBImage = image;
    }

    std::unique_ptr<GenericImageMessage> rgbmsg(new GenericImageMessage(image));
    post<freenectsourcemodule::RGBImage>(rgbmsg);
  }
}

// ######################################################################
void FreenectSourceModule::run()
{
  freenect_context *f_ctx;
  freenect_device *f_dev;

	if (freenect_init(&f_ctx, NULL) < 0) 
  { NRT_WARNING("freenect_init() failed"); return; }

	freenect_set_log_level(f_ctx, FREENECT_LOG_SPEW);
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_CAMERA));

	int nr_devices = freenect_num_devices (f_ctx);
	NRT_DEBUG ("Number of kinect devices found: " << nr_devices);

	if (nr_devices < 1)
  { NRT_WARNING("No kinect devices found - bailing out"); return; }

	int user_device_number = 0;
	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) 
  { NRT_WARNING("Could not open kinect device - bailing out"); return; }

  itsDev = f_dev;
  itsRegistration = freenect_copy_registration(itsDev);

  freenect_set_user(f_dev, this);
  
	freenect_set_depth_callback(f_dev, ::depthCallback);
	freenect_set_video_callback(f_dev, ::rgbCallback);
      
  freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
  freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_REGISTERED));

  itsFrontDepthBuffer.resize(640*480);
  itsBackDepthBuffer.resize(640*480);
  itsFrontRGBBuffer.resize(640*480*3);
  itsMidRGBBuffer.resize(640*480*3);
  itsBackRGBBuffer.resize(640*480*3);

  freenect_set_video_buffer(f_dev, &itsFrontRGBBuffer[0]);

  freenect_start_depth(f_dev);
  freenect_start_video(f_dev);

  // The freenect_process_events method needs to be called as fast as possible
  // so as not to lose packets
  bool run_freenect = true;
  std::thread freenect_thread([f_ctx, &run_freenect]()
      { while(run_freenect) { freenect_process_events(f_ctx); } });

  // Start threads to handle the RGB and Depth data as it makes its way into the mid buffers
  std::thread rgb_handler_thread(std::bind(&FreenectSourceModule::rgbHandlerThread, this));
  std::thread depth_handler_thread(std::bind(&FreenectSourceModule::depthHandlerThread, this));
  
  // Wait for shutdown
  while(running()) sleep(1);

  run_freenect = false;
  freenect_thread.join();

  itsRGBCondition.notify_all();
  rgb_handler_thread.join();

	freenect_stop_depth(f_dev);
	freenect_stop_video(f_dev);

	freenect_close_device(f_dev);
	freenect_shutdown(f_ctx);

  freenect_destroy_registration(&itsRegistration);
}

// ######################################################################
// Don't forget this to be able to use your module as a runtime-loadable shared object
NRT_REGISTER_MODULE(FreenectSourceModule);
