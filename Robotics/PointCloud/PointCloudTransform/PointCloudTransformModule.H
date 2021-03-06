/*! @file Modules/PointCloud/Common/Transform/TransformModule.H
    A module for transforming point clouds */

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
#ifndef MODULES_POINTCLOUD_COMMON_TRANSFORM_TRANSFORMMODULE_H_
#define MODULES_POINTCLOUD_COMMON_TRANSFORM_TRANSFORMMODULE_H_

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <Messages/PointCloud/GenericCloudMessage.H>

namespace pointcloud_transform_module
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(Input, GenericCloudMessage, void, "The input cloud to be transformed");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Output, GenericCloudMessage, void, "The output transformed point cloud");
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformLookup, nrt::TransformLookupMessage, nrt::TransformMessage, "The transform port");

  nrt::ParameterCategory ParamCateg("Point Cloud Transformer Related Options");
  nrt::ParameterDef<std::string> fromParamDef("from", "The 'from' transform frame", "world", ParamCateg);
  nrt::ParameterDef<std::string> toParamDef("to", "The 'to' transform frame", "world", ParamCateg);
}

//! Applies a transformation to a point cloud
/*! Processes a point cloud by applying a projective transformation to all points in the
    cloud subject to the optional subset specified by the indices.  Though we only accept
    a projective matrix (4x4), in general the actual transformation will be affine.
    \author Shane Grant

    \keywords point, cloud, transform, affine, projective */
class PointCloudTransformModule: public nrt::Module,
                                 public nrt::MessagePoster<pointcloud_transform_module::Output,
                                                           pointcloud_transform_module::TransformLookup>,
                                 public nrt::MessageSubscriber<pointcloud_transform_module::Input>
{
  public:
    PointCloudTransformModule(std::string const & instanceId="");

    void onMessage(pointcloud_transform_module::Input in);

  private:
    nrt::Parameter<std::string> itsFromParam;
    nrt::Parameter<std::string> itsToParam;
};

#endif // MODULES_POINTCLOUD_COMMON_TRANSFORM_TRANSFORMMODULE_H_
