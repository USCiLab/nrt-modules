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
// Primary maintainer for this file: Shane Grant
//
#ifndef MODULES_POINTCLOUD_IO_WORLDDISPLAY_WORLDDISPLAYMODULE_H_
#define MODULES_POINTCLOUD_IO_WORLDDISPLAY_WORLDDISPLAYMODULE_H_

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/PointCloud/IO/PointCloudSink/PointCloudSink.H>
#include <Messages/PointCloud/GenericCloudMessage.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <nrt/PointCloud/Utility/GLWindowPointCloud.H>



namespace worlddisplay
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(Cloud, GenericCloudMessage, void, "Input Cloud");
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformLookup, nrt::TransformLookupMessage, nrt::TransformMessage, "The transform lookup port");

  static const nrt::ParameterCategory ParamCategory("World Display Sink Module Related Options");
  static const nrt::ParameterDef<std::string> baseParamDef("base", "The base transform", "world");
  static const nrt::ParameterDef<std::string> transformsParamDef("transforms", "A comma separated list of transforms to display", "");
}

//! Displays point clouds and transforms in a single opengl window
/*! 
    \author Randolph Voorhies

    \keywords display, point, cloud, 3d, world, transform, transformation*/
class WorldDisplayModule: public nrt::Module,
                          public nrt::MessageSubscriber<worlddisplay::Cloud>,
                          public nrt::MessagePoster<worlddisplay::TransformLookup>
{
  public:
    WorldDisplayModule(std::string const& instanceid = "");

    void onMessage( worlddisplay::Cloud msg );

    void run();

  private:
    std::mutex itsMtx;
    void drawFrames();
    std::vector<std::string> itsTransformNames;
    std::map<std::string, nrt::Transform3D> itsTransforms;
    void transformParamCallback(std::string const & transforms);
    nrt::GLWindowPointCloud itsWindow;
    nrt::Parameter<std::string> itsBaseParam;
    nrt::Parameter<std::string> itsTransformsParam;
};

#endif // MODULES_POINTCLOUD_IO_WORLDDISPLAY_WORLDDISPLAYMODULE_H_
