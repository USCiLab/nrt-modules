/*! @file Modules/PointCloud/IO/PointCloudSource/PointCloudSourceModule.C
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

#include "PointCloudSourceModule.H"
#include <nrt/PointCloud/IO/PointCloudSource/details/SeekablePointCloudSourceType.H>
#include <chrono>
#include <thread> 

// ######################################################################
PointCloudSourceModule::PointCloudSourceModule(std::string const& instanceid) :
  nrt::Module(instanceid),
  itsPointCloudSource(new nrt::PointCloudSource("source")),
  itsFramerateParam(pointcloudsource::ParamDefFrameRate, this),
  itsLoopParam(pointcloudsource::ParamDefLoop, this)
{
  addSubComponent(itsPointCloudSource);
}

// ######################################################################
void PointCloudSourceModule::run()
{
  nrt::int32 seqCount = 0;

  while( running() )
  {
    auto startTime = std::chrono::monotonic_clock::now();
    float const framerate = itsFramerateParam.getVal();

    if(framerate == 0) { usleep(100000); continue; }

    auto delayTime = std::chrono::milliseconds(static_cast<int>(1000.0F/framerate));
    auto endTime = startTime + delayTime;

    if ( itsPointCloudSource->ok() )
    {
      // First, post a clock tick:
      std::unique_ptr<nrt::TriggerMessage> tickMsg(new nrt::TriggerMessage);
      post<pointcloudsource::Tick>(tickMsg); // this will block until completion of all callbacks

      // Get the next cloud:
      nrt::GenericCloud const cloud = itsPointCloudSource->in();

      // Post the cloud count:
      std::unique_ptr<nrt::Message<nrt::int32> > seqMsg(new nrt::Message<nrt::int32>( seqCount ));
      post<pointcloudsource::CloudSeqCount>(seqMsg); // this will block until completion of all callbacks

      // Post the size of our cloud:
      std::unique_ptr<nrt::Message<size_t> >
      sizeMsg(new nrt::Message<size_t>( cloud.size() ) );
      post<pointcloudsource::Size>(sizeMsg); // this will block until completion of all callbacks

      // Post the cloud itself:
      std::unique_ptr<GenericCloudMessage> cloudMsg(new GenericCloudMessage(cloud));
      post<pointcloudsource::Cloud>(cloudMsg); // this will block until completion of all callbacks

      // tock tock:
      std::unique_ptr<nrt::TriggerMessage> tockMsg(new nrt::TriggerMessage);
      post<pointcloudsource::Tock>(tockMsg); // this will block until completion of all callbacks

      // Increment the cloud count:
      ++seqCount;
    }
    else
    {
      // Get a reference to the real image source
      std::shared_ptr<nrt::SeekablePointCloudSourceType> fileSource =
        std::dynamic_pointer_cast<nrt::SeekablePointCloudSourceType>(itsPointCloudSource->actualSource());

      // If it is a file source, then either seek back to the first frame, or break:
      if (fileSource)
      {
        if (itsLoopParam.getVal())
        {
          NRT_INFO("Looping back to cloud #" << fileSource->cloudRange().first);
          fileSource->seek(fileSource->cloudRange().first);
        }
        else break; // If not looping, then stop when we've reached the end
      }
      else
      {
        break; // Our stream has been broken, let's get out of here.
      }
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
  post<pointcloudsource::Done>(donemsg);
}

// Don't forget this to be able to use your module as a runtime-loadable shared object
NRT_REGISTER_MODULE(PointCloudSourceModule);
