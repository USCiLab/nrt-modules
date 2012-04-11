/*! @file Modules/PointCloud/Filter/FilterModule.H
    A module for filtering point clouds */

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
#ifndef MODULES_POINTCLOUD_FILTER_FILTERMODULE_H_
#define MODULES_POINTCLOUD_FILTER_FILTERMODULE_H_

#include <nrt/Core/Blackboard/Module.H>
#include "Filters/PointCloudFilters.H"
#include <Messages/PointCloud/GenericCloudMessage.H>
#include <Messages/PointCloud/IndicesMessage.H>

namespace pointcloud_filter_module
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT( Input, GenericCloudMessage, void, "The input cloud to be filtered" );
  NRT_DECLARE_MESSAGECHECKER_PORT( Indices, IndicesMessage, "The optional indices describing a subset of the cloud to filter" );
  NRT_DECLARE_MESSAGEPOSTER_PORT( Output, GenericCloudMessage, void, "The output filtered point cloud" );
}

//! Filters a point cloud
/*! Processes a point cloud by filtering it in varying manners such as outlier removal,
    region removal, or voxelization.

    Many filters use a specific field in a point cloud to perform filtering, such
    as the geometry, point normals, or color information.  Though only this field
    will determine which points are filtered, all attributes related to a point will
    be affected.

    \author Shane Grant

    \keywords point, cloud, voxel, radius, statistical, outlier, pass, through, random */
class PointCloudFilterModule: public nrt::Module,
  public nrt::MessagePoster<pointcloud_filter_module::Output>,
  public nrt::MessageSubscriber<pointcloud_filter_module::Input>,
  public nrt::MessageChecker<pointcloud_filter_module::Indices>
{
  public:
    PointCloudFilterModule( std::string const & instanceId = "pointcloud filter module" );

    void onMessage( pointcloud_filter_module::Input in );

  private:
    std::shared_ptr<PointCloudFilter> itsFilter;
};

#endif // MODULES_POINTCLOUD_IO_DEPTHIMAGETOPOINTCLOUD_DEPTHIMAGETOPOINTCLOUDMODULE_H_
