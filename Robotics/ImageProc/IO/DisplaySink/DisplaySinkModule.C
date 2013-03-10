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

#include "DisplaySinkModule.H"
#include <functional>
#include <nrt/ImageProc/Drawing/Text.H>
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>

// ######################################################################
DisplaySinkModule::DisplaySinkModule(std::string const & instanceid) :
    nrt::Module(instanceid),
    itsDisplaySink(new nrt::DisplayImageSink())
{ 
  addSubComponent(itsDisplaySink);
  itsDisplaySink->setKeyCallback(std::bind(&DisplaySinkModule::onKeyPress, this, std::placeholders::_1));
  itsDisplaySink->setClickCallback(std::bind(&DisplaySinkModule::onMouseClick, this,
        std::placeholders::_1, std::placeholders::_2));
  
}

// ######################################################################
void DisplaySinkModule::onMessage(displaysink::Image msg)
{
  std::string label;
  if (nrt::MessageCheckerResults<nrt::Message<std::string>> res = check<displaysink::Label>(nrt::MessageCheckerPolicy::Any))
    while (res.exhausted() == false)
      if (std::shared_ptr<nrt::Message<std::string> const> msg = res.get()) {
        label = msg->value();
      }

  itsDisplaySink->out(msg->img);
}

// ######################################################################
void DisplaySinkModule::onMouseClick(nrt::Point2D<nrt::int32> point, nrt::int32 button)
{
  std::unique_ptr<nrt::Message<nrt::Point2D<nrt::int32>>> msg(new nrt::Message<nrt::Point2D<nrt::int32>>(point));
  post<displaysink::MouseClick>(msg);
}

// ######################################################################
void DisplaySinkModule::onKeyPress(nrt::int32 key)
{
  std::unique_ptr<nrt::Message<nrt::int32>> msg(new nrt::Message<nrt::int32>(key));
  post<displaysink::KeyboardPress>(msg);
}

// Don't forget this to be able to use your module as a runtime-loadable shared object
NRT_REGISTER_MODULE(DisplaySinkModule);

