/*! @file PointCloud/FreenectSource/FreenectSourceModule.C */

#include "FreenectSourceModule.H"
#include <libfreenect/libfreenect-registration.h>
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
void FreenectSourceModule::depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
  //std::lock_guard<std::mutex> _(itsBuffMtx);
  //memcpy(&itsFrontDepthBuffer[0], v_depth, sizeof(uint16_t)*640*480);

	//uint16_t const * const depth = (uint16_t*)v_depth;
  //uint16_t const * depth_in_ptr = depth;
  //nrt::PixRGB<uint8_t> const * rgb_ptr = itsRGBImage.const_begin();

  //nrt::Image<nrt::PixGray<float>> depthImage(640, 480);
  //float * depth_out_ptr = depthImage.pod_begin();

  //nrt::PointCloud<nrt::PointXYZRGBAF> cloud;

  //double world_x = 0;
  //double world_y = 0;
  //for(int cam_y=0; cam_y<480; ++cam_y)
  //  for(int cam_x=0; cam_x<640; ++cam_x)
  //  {
  //    double const world_z = (*depth_in_ptr);
  //    freenect_camera_to_world(dev, cam_x, cam_y, world_z, &world_x, &world_y);

  //    (*depth_out_ptr) = (*depth_in_ptr) / 100.0;

  //    nrt::PointXYZRGBAF point(nrt::Point3DEd(world_x / 100.0, world_y / 100.0, world_z / 100.0), nrt::PixRGBA<float>(*rgb_ptr));
  //    cloud.insert(point);

  //    ++depth_in_ptr;
  //    ++depth_out_ptr;
  //    ++rgb_ptr;
  //  }

  //std::unique_ptr<GenericImageMessage> depthmsg(new GenericImageMessage(depthImage));
  //post<freenectsourcemodule::DepthImage>(depthmsg);

  //std::unique_ptr<GenericCloudMessage> cloudmsg(new GenericCloudMessage(cloud));
  //post<freenectsourcemodule::Cloud>(cloudmsg);

  //itsDepthImage = depthImage;
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

    nrt::Image<nrt::PixRGB<uint8_t>> image(640, 480);
    memcpy(image.pod_begin(), &itsBackRGBBuffer[0], sizeof(char)*640*480*3);
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

  freenect_set_user(f_dev, this);
  
	freenect_set_depth_callback(f_dev, ::depthCallback);
	freenect_set_video_callback(f_dev, ::rgbCallback);
      
  freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
  freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_REGISTERED));

  itsFrontDepthBuffer.resize(640*480);
  itsMidDepthBuffer.resize(640*480);
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

  // Start a thread to handle the RGB data as it makes its way into the mid buffer
  std::thread rgb_handler_thread(std::bind(&FreenectSourceModule::rgbHandlerThread, this));
  



  while(running()) 
  { 
    //bool newRGB;
    //{
    //  std::lock_guard<std::mutex> _(itsRGBMtx);
    //  newRGB = itsNewRGB;
    //  if(newRGB) 
    //  {
    //    std::swap(itsMidRGBBuffer, itsBackRGBBuffer);
    //    itsNewRGB = false;
    //  }
    //}

    //if(newRGB)
    //{
    //  nrt::Image<nrt::PixRGB<uint8_t>> image(640, 480);
    //  memcpy(image.pod_begin(), &itsBackRGBBuffer[0], sizeof(char)*640*480*3);
    //  std::unique_ptr<GenericImageMessage> rgbmsg(new GenericImageMessage(image));
    //  post<freenectsourcemodule::RGBImage>(rgbmsg);
    //}
    
    sleep(1);
  }

  NRT_INFO("Done Running... Shutting Down");

  run_freenect = false;
  freenect_thread.join();

	freenect_stop_depth(f_dev);
	freenect_stop_video(f_dev);

	freenect_close_device(f_dev);
	freenect_shutdown(f_ctx);
  NRT_INFO("Finished shutting down...");
}

// ######################################################################
// Don't forget this to be able to use your module as a runtime-loadable shared object
NRT_REGISTER_MODULE(FreenectSourceModule);
