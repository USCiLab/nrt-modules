/*! @file Modules/PointCloud/IO/DepthImageToPointCloud/DepthImageToPointCloudModule.H
    A module for making clouds from depth images */

// //////////////////////////////////////////////////////////////////// //
// The NRT iLab Neuromorphic Vision C++ Toolkit, Copyright(C) 2000-2011 //
// by the University of Southern California (USC) and the iLab at USC.  //
// See http://iLab.usc.edu for information about this project.          //
// //////////////////////////////////////////////////////////////////// //
// Portions of the NRT iLab Neuromorphic Vision Toolkit are protected   //
// under the U.S. patent ``Computation of Intrinsic Perceptual Saliency //
// in Visual Environments, and Applications'' by Christof Koch and      //
// Laurent Itti, California Institute of Technology, 2001 (patent       //
// pending; application number 09/912,225 filed July 23, 2001; see      //
// http://pair.uspto.gov/cgi-bin/final/home.pl for current status).     //
// //////////////////////////////////////////////////////////////////// //
// This file is part of the NRT iLab Neuromorphic Vision C++ Toolkit.   //
//                                                                      //
// The NRT iLab Neuromorphic Vision C++ Toolkit is free software; you   //
// can redistribute it and/or modify it under the terms of the GNU      //
// General Public License as published by the Free Software Foundation; //
// either version 2, or (at your option) any later version.             //
//                                                                      //
// The NRT iLab Neuromorphic Vision C++ Toolkit is distributed in the   //
// hope that it will be useful, but WITHOUT ANY WARRANTY; without even  //
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  //
// PURPOSE.  See the GNU General Public License for more details.       //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with the NRT iLab Neuromorphic Vision C++ Toolkit; if not,     //
// write to the Free Software Foundation, Inc., 59 Temple Place, Suite  //
// 330, Boston, MA 02111-1307 USA.                                      //
// //////////////////////////////////////////////////////////////////// //
//
// Primary maintainer for this file: Shane Grant <wgrant@usc.edu>

#ifndef MODULES_POINTCLOUD_IO_DEPTHIMAGETOPOINTCLOUD_DEPTHIMAGETOPOINTCLOUDMODULE_H_
#define MODULES_POINTCLOUD_IO_DEPTHIMAGETOPOINTCLOUD_DEPTHIMAGETOPOINTCLOUDMODULE_H_

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/PointCloud/GenericCloud.H>
#include <Messages/PointCloud/GenericCloudMessage.H>
#include <Messages/ImageProc/GenericImageMessage.H>

namespace pointcloud_depth_to_image
{
	NRT_DECLARE_MESSAGESUBSCRIBER_PORT(Input,     GenericImageMessage, void, "The input RGB-D or grayscale depth image");
	NRT_DECLARE_MESSAGECHECKER_PORT(FocalLength,  nrt::Message<nrt::real>,   "The focal length (in pixels) of the input source");
	NRT_DECLARE_MESSAGEPOSTER_PORT(Output,        GenericCloudMessage, void, "The output point cloud generated from the depth image");
}

//! Creates a point cloud from a depth image
/*! A point cloud can be generated from either a depth image with color information
 		(PixRGBD) or without (PixGray) as long as the focal length (in pixels) of the camera
		that generated the image is available.  Depth information should be in meters.

		If color information is present in the input image, it will automatically be
		included in the output cloud.

    \author Shane Grant

    \keywords point, cloud, depth, convert, image, rgbd */
class DepthImageToPointCloudModule: public nrt::Module,
	                                  public nrt::MessagePoster<pointcloud_depth_to_image::Output>,
	                                  public nrt::MessageChecker<pointcloud_depth_to_image::FocalLength>,
	                                  public nrt::MessageSubscriber<pointcloud_depth_to_image::Input>
{
	public:
		DepthImageToPointCloudModule( std::string const & instanceId = "");

		void onMessage(pointcloud_depth_to_image::Input in);
};

#endif // MODULES_POINTCLOUD_IO_DEPTHIMAGETOPOINTCLOUD_DEPTHIMAGETOPOINTCLOUDMODULE_H_
