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
// Primary maintainer for this file: Randolph Voorhies <voorhies@usc.edu>
#ifndef MODULES_POINTCLOUD_POINTCLOUD2OCCUPANCY_POINTCLOUD2OCCUPANCYMODULE_H
#define MODULES_POINTCLOUD_POINTCLOUD2OCCUPANCY_POINTCLOUD2OCCUPANCYMODULE_H

#include <nrt/Core/Blackboard/Module.H>
#include <Messages/PointCloud/GenericCloudMessage.H>
#include <Messages/ImageProc/GenericImageMessage.H>

namespace pointcloud2occupancy
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT( PointCloudInput, GenericCloudMessage, void, "The input cloud" );
  NRT_DECLARE_MESSAGEPOSTER_PORT( OccupancyGridOutput, GenericImageMessage, void, "The occupancy grid" );

  nrt::ParameterCategory paramCat("Point Cloud -> Occupancy Grid Related Options");

  nrt::ParameterDef<nrt::real> lowerThresholdParamDef("lowerthreshold",
      "The lower z threshold (in meters). Points with z coordinates lower than this will be ignored", .2, paramCat);

  nrt::ParameterDef<nrt::real> upperThresholdParamDef("upperthreshold",
      "The upper z threshold (in meters). Points with z coordinates higher than this will be ignored", 3, paramCat);

  nrt::ParameterDef<nrt::real> pixelsPerMeterParamDef("pixelspermeter",
      "The pixels/meter scale of the map", 10, paramCat);

  nrt::ParameterDef<nrt::Dims<float>> mapDimsParamDef("mapdims",
      "The dimensions of the occupancy grid (in meters)", nrt::Dims<float>(8, 8), paramCat);
}

//! Turns a point cloud into an occupancy grid
/*! Useful for 2D path planning, this module simply projects all points in a cloud between two thresholds onto the X/Y plane, and
    quatizes them into a grid

    \author Randolph Voorhies

    \keywords point, cloud, voxel, grid, occupancy grid, occupancy, path planning, traversability */
class PointCloud2OccupancyModule: public nrt::Module,
  public nrt::MessageSubscriber<pointcloud2occupancy::PointCloudInput>,
  public nrt::MessagePoster<pointcloud2occupancy::OccupancyGridOutput>
{
  public:
    PointCloud2OccupancyModule( std::string const & instanceId = "" );

    void onMessage( pointcloud2occupancy::PointCloudInput cloud );

  private:
    std::mutex itsMtx;
    nrt::Parameter<nrt::real> itsLowerThreshParam;
    nrt::Parameter<nrt::real> itsUpperThreshParam;
    nrt::Parameter<nrt::real> itsPixelsPerMeterParam;
    nrt::Parameter<nrt::Dims<float>> itsMapDimsParam;
};

#endif // MODULES_POINTCLOUD_POINTCLOUD2OCCUPANCY_POINTCLOUD2OCCUPANCYMODULE_H

