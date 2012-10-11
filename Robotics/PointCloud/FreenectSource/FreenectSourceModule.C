/*! @file PointCloud/FreenectSource/FreenectSourceModule.C */

#include "FreenectSourceModule.H"
#include <libfreenect/libfreenect-registration.h>
#include <libfreenect.hpp>

// ######################################################################
FreenectSourceModule::FreenectSourceModule(std::string const & instanceName) :
    nrt::Module(instanceName)
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
	uint16_t const * const depth = (uint16_t*)v_depth;
  uint16_t const * depth_ptr = depth;

  
  double world_x = 0;
  double world_y = 0;
  for(int cam_y=0; cam_y<480; ++cam_y)
    for(int cam_x=0; cam_x<640; ++cam_x)
    {
      double const world_z = (*depth_ptr);
      freenect_camera_to_world(dev, cam_x, cam_y, world_z, &world_x, &world_y);

      ++depth_ptr;
    }
  NRT_INFO("Got Depth Image");
}

// ######################################################################
void rgbCallback(freenect_device *dev, void *rgb, uint32_t timestamp)
{
  FreenectSourceModule * module = static_cast<FreenectSourceModule*>(freenect_get_user(dev));
  module->rgbCallback(dev, rgb, timestamp);
}
void FreenectSourceModule::rgbCallback(freenect_device *dev, void *rgb, uint32_t timestamp)
{
	//pthread_mutex_lock(&gl_backbuf_mutex);

  nrt::Image<nrt::PixRGB<uint8_t>> image(640, 480);
  memcpy(image.pod_begin(), rgb, sizeof(char)*640*480*3);
  
  std::unique_ptr<GenericImageMessage> rgbmsg(new GenericImageMessage(image));
  post<freenectsourcemodule::RGBImage>(rgbmsg);

	//// swap buffers
	//assert (rgb_back == rgb);
	//rgb_back = rgb_mid;
	//freenect_set_video_buffer(dev, rgb_back);
	//rgb_mid = (uint8_t*)rgb;

	//got_rgb++;
	//pthread_cond_signal(&gl_frame_cond);
	//pthread_mutex_unlock(&gl_backbuf_mutex);
  NRT_INFO("Got RGB Image");
}


// ######################################################################
void FreenectSourceModule::run()
{
  freenect_context *f_ctx;
  freenect_device *f_dev;

	if (freenect_init(&f_ctx, NULL) < 0) 
  {
		NRT_WARNING("freenect_init() failed");
		return;
	}

	freenect_set_log_level(f_ctx, FREENECT_LOG_SPEW);
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_CAMERA));

	int nr_devices = freenect_num_devices (f_ctx);
	NRT_DEBUG ("Number of kinect devices found: " << nr_devices);

	if (nr_devices < 1)
  {
    NRT_WARNING("No kinect devices found - bailing out");
		return;
  }

	int user_device_number = 0;
	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		NRT_WARNING("Could not open kinect device - bailing out");
		return;
	}

  freenect_set_user(f_dev, this);
  
	freenect_set_depth_callback(f_dev, ::depthCallback);
	freenect_set_video_callback(f_dev, ::rgbCallback);
      
  freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
  freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_REGISTERED));

  std::vector<uint8_t> rgb_buffer(648*480*3);
  freenect_set_video_buffer(f_dev, &rgb_buffer[0]);

  freenect_start_depth(f_dev);
  freenect_start_video(f_dev);
  

    //std::unique_ptr<GenericImageMessage> rgbmsg(new GenericImageMessage(rgbImage));
    //post<freenectsourcemodule::RGBImage>(rgbmsg);
    //
  while(running())
  {
    freenect_process_events(f_ctx);
  }

	freenect_stop_depth(f_dev);
	freenect_stop_video(f_dev);

	freenect_close_device(f_dev);
	freenect_shutdown(f_ctx);
}

// ######################################################################
// Don't forget this to be able to use your module as a runtime-loadable shared object
NRT_REGISTER_MODULE(FreenectSourceModule);
