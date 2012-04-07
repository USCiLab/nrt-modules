/*! @file Modules/PointCloud/IO/opennisourcemodule/OpenNISourceModule.C
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

#include "OpenNISourceModule.H"
#include <chrono>
#include <thread>

// ######################################################################
OpenNISourceModule::OpenNISourceModule(std::string const& instanceid) :
  nrt::Module(instanceid),
  itsOpenNISource(new nrt::OpenNIImageSource("openni source")),
  itsFramerateParam(opennisourcemodule::ParamDefFrameRate, this)
{
  addSubComponent(itsOpenNISource);
}

// ######################################################################
void OpenNISourceModule::run()
{
  nrt::int32 seqCount = 0;

  while( running() )
  {
    auto startTime = std::chrono::monotonic_clock::now();
    float const framerate = itsFramerateParam.getVal();

    if(framerate == 0) { usleep(100000); continue; }

    auto delayTime = std::chrono::milliseconds(static_cast<int>(1000.0F/framerate));
    auto endTime = startTime + delayTime;

    if ( itsOpenNISource->ok() )
    {
      // First, post a clock tick:
      std::unique_ptr<nrt::TriggerMessage> tickMsg(new nrt::TriggerMessage);
      post<opennisourcemodule::Tick>(tickMsg); // this will block until completion of all callbacks

      // Get the next image:
      nrt::GenericImage const image = itsOpenNISource->in();

      // Post the frame count:
      std::unique_ptr<nrt::Message<nrt::int32> > seqMsg(new nrt::Message<nrt::int32>( seqCount ));
      post<opennisourcemodule::FrameCount>(seqMsg); // this will block until completion of all callbacks

      // Post the dims
      std::unique_ptr<nrt::Message<nrt::GenericImage::DimsType> >
      sizeMsg(new nrt::Message<nrt::GenericImage::DimsType>( image.dims() ) );
      post<opennisourcemodule::Dims>(sizeMsg); // this will block until completion of all callbacks

			// Post metadata
			std::unique_ptr<nrt::Message<nrt::real>>
				focalLengthMsg( new nrt::Message<nrt::real>( itsOpenNISource->getDeviceData().Image.focalLength ) );
			post<opennisourcemodule::FocalLength>( focalLengthMsg );

      // Post the image itself:
      std::unique_ptr<GenericImageMessage> imMsg(new GenericImageMessage(image));
      post<opennisourcemodule::Image>(imMsg);

      // tock tock:
      std::unique_ptr<nrt::TriggerMessage> tockMsg(new nrt::TriggerMessage);
      post<opennisourcemodule::Tock>(tockMsg);

      // Increment the cloud count:
      ++seqCount;
    }

    // Maintain the framerate
    if(framerate > 0)
    {
      if (std::chrono::monotonic_clock::now() > startTime + delayTime)
        NRT_WARNING("Cannot maintain framerate");
      else std::this_thread::sleep_until(endTime);
    }
  }

  std::unique_ptr<nrt::TriggerMessage> donemsg(new nrt::TriggerMessage);
  post<opennisourcemodule::Done>(donemsg);
}

// Don't forget this to be able to use your module as a runtime-loadable shared object
NRT_REGISTER_MODULE(OpenNISourceModule);
