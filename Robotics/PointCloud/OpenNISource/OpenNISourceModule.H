/*! @file Modules/ImageProc/IO/OpenNISourceModule/OpenNISourceModule.H
    A OpenNI data source module */

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

#ifndef MODULES_IMAGEPROC_IO_IMAGESOURCE_OPENNISOURCE_OPENNISOURCEMODULE_H_
#define MODULES_IMAGEPROC_IO_IMAGESOURCE_OPENNISOURCE_OPENNISOURCEMODULE_H_

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/ImageProc/IO/ImageSource/OpenNIImageSource.H>
#include "Messages/ImageProc/GenericImageMessage.H"

namespace opennisourcemodule
{
  static nrt::ParameterCategory Options("OpenNI Related Options");

  static nrt::ParameterDef<float> ParamDefFrameRate("framerate",
      "The framerate at which to publish data (in Hz), or -1 to post as fast as possible.", -1, Options);

  NRT_DECLARE_MESSAGEPOSTER_PORT(Image,         GenericImageMessage,                       void, "Output Image");
	NRT_DECLARE_MESSAGEPOSTER_PORT(FocalLength,		nrt::Message<nrt::real>,		  						 void, "Sensor Focal Length");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Dims,          nrt::Message<nrt::GenericImage::DimsType>, void, "Frame Dimensions");
  NRT_DECLARE_MESSAGEPOSTER_PORT(FrameCount,    nrt::Message<nrt::int32>,                  void, "Frame Count" );
  NRT_DECLARE_MESSAGEPOSTER_PORT(Tick,          nrt::TriggerMessage,                       void, "Data Tick");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Tock,          nrt::TriggerMessage,                       void, "Data Tock");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Done,          nrt::TriggerMessage,                       void, "No more input");
}

//! Gets data from an OpenNI compatible piece of hardware
/*! This data source is capable of grabbing data from an OpenNI
    compatible piece of hardware such as the Kinect.  It can be used
    to get various types of images as well as hardware metadata
    such as focal length information.

    @author Shane Grant

    @keywords depth, sensor, kinect, image, source */
class OpenNISourceModule: public nrt::Module,
  public nrt::MessagePoster<opennisourcemodule::Image,
                            opennisourcemodule::FocalLength,
                            opennisourcemodule::Dims,
                            opennisourcemodule::FrameCount,
                            opennisourcemodule::Tick,
                            opennisourcemodule::Tock,
                            opennisourcemodule::Done>
{
  public:
    OpenNISourceModule(std::string const& instanceid = "");

    void run();

  private:
    std::shared_ptr<nrt::OpenNIImageSource> itsOpenNISource;
    nrt::Parameter<float> itsFramerateParam;
};

#endif // MODULES_IMAGEPROC_IO_IMAGESOURCE_OPENNISOURCE_OPENNISOURCEMODULE_H_
