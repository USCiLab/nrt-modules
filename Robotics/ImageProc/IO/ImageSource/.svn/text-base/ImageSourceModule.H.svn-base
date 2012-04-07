//NRT_HEADER_BEGIN
/*! @file ImageSource/ImageSourceModule.H */

// ////////////////////////////////////////////////////////////////////////
//              The iLab Neuromorphic Robotics Toolkit (NRT)             //
// Copyright 2010 by the University of Southern California (USC) and the //
//                              iLab at USC.                             //
//                                                                       //
//                iLab - University of Southern California               //
//                Hedco Neurociences Building, Room HNB-10               //
//                    Los Angeles, Ca 90089-2520 - USA                   //
//                                                                       //
//      See http://ilab.usc.edu for information about this project.      //
// ////////////////////////////////////////////////////////////////////////
// This file is part of The iLab Neuromorphic Robotics Toolkit.          //
//                                                                       //
// The iLab Neuromorphic Robotics Toolkit is free software: you can      //
// redistribute it and/or modify it under the terms of the GNU General   //
// Public License as published by the Free Software Foundation, either   //
// version 3 of the License, or (at your option) any later version.      //
//                                                                       //
// The iLab Neuromorphic Robotics Toolkit is distributed in the hope     //
// that it will be useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       //
// PURPOSE.  See the GNU General Public License for more details.        //
//                                                                       //
// You should ave received a copy of the GNU General Public License     //
// along with The iLab Neuromorphic Robotics Toolkit.  If not, see       //
// <http://www.gnu.org/licenses/>.                                       //
// ////////////////////////////////////////////////////////////////////////
//
// Primary maintainer for this file: Randolph Voorhies
//
//NRT_HEADER_END

#ifndef IMAGE_IO_IMAGESOURCE_IMAGESOURCEMODULE_H
#define IMAGE_IO_IMAGESOURCE_IMAGESOURCEMODULE_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/ImageProc/IO/ImageSource/ImageSource.H>

#include "Messages/ImageProc/GenericImageMessage.H"

namespace imagesource
{
  static nrt::ParameterCategory ParamCateg("ImageSource Related Options");

  static nrt::ParameterDef<float> ParamDefFrameRate("framerate",
      "The framerate at which to publish images (in Hz), or -1 to post as fast as possible.", -1, ParamCateg);

  static nrt::ParameterDef<bool> ParamDefLoop("loop", "Start the video over when completed?", false, ParamCateg);

  NRT_DECLARE_MESSAGEPOSTER_PORT(Image,         GenericImageMessage,                       void, "Output Frames");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Dims,          nrt::Message<nrt::GenericImage::DimsType>, void, "Frame Dimensions");
  NRT_DECLARE_MESSAGEPOSTER_PORT(FrameCount,    nrt::Message<int>,                         void, "Frame Count");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Tick,          nrt::TriggerMessage,                       void, "Frame Tick");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Tock,          nrt::TriggerMessage,                       void, "Frame Tock");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Done,          nrt::TriggerMessage,                       void, "No more input");
}

//! Produces Images from a selected source
/*! The Image Source is a configurable, general purpose source for Images.
    By selecting the correct setting for the 'in' parameter, you can choose
    to grab Images from video files, sequences of image files, cameras, etc.

    \author Randolph Voorhies

    \keywords camera, video, images, mpg, mpeg, jpg, jpeg, png */
class ImageSourceModule: public nrt::Module,
                         public nrt::MessagePoster<imagesource::Image,
                                                   imagesource::Dims,
                                                   imagesource::FrameCount,
                                                   imagesource::Tick,
                                                   imagesource::Tock,
                                                   imagesource::Done>
{
  public:
    ImageSourceModule(std::string const& instanceid = "");
    
    void run();
    
  private:
    std::shared_ptr<nrt::ImageSource> itsImageSource;
    nrt::Parameter<float> itsFramerateParam;
    nrt::Parameter<bool>  itsLoopParam;
};

#endif // IMAGE_IO_IMAGESOURCE_IMAGESOURCEMODULE_H
