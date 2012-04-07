/*! @file Modules/PointCloud/IO/PointCloudSource/PointCloudSourceModule.H 
 		A point cloud source module */

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

#ifndef MODULES_POINTCLOUD_IO_POINTCLOUDSOURCE_POINTCLOUDSOURCEMODULE_H_
#define MODULES_POINTCLOUD_IO_POINTCLOUDSOURCE_POINTCLOUDSOURCEMODULE_H_

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/PointCloud/IO/PointCloudSource/PointCloudSource.H>
#include <Messages/PointCloud/GenericCloudMessage.H>

namespace pointcloudsource
{
  static nrt::ParameterCategory ParamCateg("PointCloudSource Related Options");

  static nrt::ParameterDef<float> ParamDefFrameRate("framerate",
      "The framerate at which to publish clouds (in Hz), or -1 to post as fast as possible.", -1, ParamCateg);

  static nrt::ParameterDef<bool> ParamDefLoop("loop", "Start the stream of clouds over when completed?", false, ParamCateg);

  NRT_DECLARE_MESSAGEPOSTER_PORT(Cloud,         GenericCloudMessage,                       void, "Output Cloud(s)");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Size,          nrt::Message<size_t>, 										 void, "Cloud Size");
  NRT_DECLARE_MESSAGEPOSTER_PORT(CloudSeqCount, nrt::Message<nrt::int32>,                  void, "Cloud Sequence Count");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Tick,          nrt::TriggerMessage,                       void, "Frame Tick");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Tock,          nrt::TriggerMessage,                       void, "Frame Tock");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Done,          nrt::TriggerMessage,                       void, "No more input");
}

//! Produces PointClouds from a point cloud source
/*! The PointCloud source is a general purpose source for PointClouds.
		By selecting the appropriate setting for the 'in' parameter, you can
		grab PointClouds from text files, ply files, sequences of files,
		depth cameras, etc.

		@author Shane Grant

		@keywords depth, sensor, kinect, ply, point, cloud */
class PointCloudSourceModule: public nrt::Module,
                              public nrt::MessagePoster<pointcloudsource::Cloud,
                                                        pointcloudsource::Size,
                                                        pointcloudsource::CloudSeqCount,
                                                        pointcloudsource::Tick,
                                                        pointcloudsource::Tock,
                                                        pointcloudsource::Done>
{
  public:
    PointCloudSourceModule(std::string const& instanceid = "");
    
    void run();
    
  private:
    std::shared_ptr<nrt::PointCloudSource> itsPointCloudSource;
    nrt::Parameter<float> itsFramerateParam;
    nrt::Parameter<bool>  itsLoopParam;
};

#endif // MODULES_POINTCLOUD_IO_POINTCLOUDSOURCE_POINTCLOUDSOURCEMODULE_H_
