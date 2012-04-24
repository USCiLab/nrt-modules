/*! @file Modules/PointCloud/IO/DepthImageToPointCloud/DepthImageToPointCloudModule.C
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

#include "DepthImageToPointCloudModule.H"

using namespace pointcloud_depth_to_image;

DepthImageToPointCloudModule::DepthImageToPointCloudModule( std::string const & instanceId ) :
  nrt::Module( instanceId )
{ }

void DepthImageToPointCloudModule::onMessage( Input in )
{
  // Check for a focal length
  float focal;
  if( auto res = check<FocalLength>(nrt::MessageCheckerPolicy::Any) )
  {
    focal = res.get()->value; 
  }
  else 
  {
    NRT_WARNING("No Focal Length Available! Skipping Processing");
    return; 
  }

  // Create the point cloud
	nrt::GenericCloud output;
	nrt::GenericImage input = in->img;

  // keep in same format
  if( input.hasPixels<nrt::PixRGBD<float>>() )
    output = nrt::PointCloud<nrt::PointXYZRGBAF>( input.get<nrt::PixRGBD<float>>(), focal );
  else if( input.hasPixels<nrt::PixGray<float>>() )
    output = nrt::PointCloud<nrt::PointXYZF>( input.get<nrt::PixGray<float>>(), focal );
  else
  {
    NRT_WARNING( "Unsupported pixel type" );
    return;
  }

	// post the cloud
  std::unique_ptr<GenericCloudMessage> cloudMsg( new GenericCloudMessage( output ) );
  post<Output>( cloudMsg );
}

// Don't forget this to be able to use your module as a runtime-loadable shared object
NRT_REGISTER_MODULE(DepthImageToPointCloudModule);
