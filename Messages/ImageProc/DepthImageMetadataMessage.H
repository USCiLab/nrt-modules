/*! @file Messages/ImageProc/DepthImageMetadataMessage.H A message containing
 		relevant metadata for depth images such as sensor data */

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

#ifndef MESSAGES_IMAGEPROC_DEPTHIMAGEMETADATAMESSAGE_H
#define MESSAGES_IMAGEPROC_DEPTHIMAGEMETADATAMESSAGE_H

#include <nrt/Core/Blackboard/Message.H>
#include <nrt/ImageProc/IO/ImageSource/OpenNIImageSource.H>

class DepthImageMetadataMessage : public nrt::MessageBase
{
  public:
		typedef nrt::OpenNIImageSource::OpenNIData OpenNIData;

    DepthImageMetadataMessage() { }
		DepthImageMetadataMessage( OpenNIData const & data ) : itsOpenNIData( data ) { }
    virtual ~DepthImageMetadataMessage() { }

		//! Gets the focal length associated with the depth camera that generated this
		inline float getDepthFocalLength() const;

		// Data from an OpenNI Device
		OpenNIData itsOpenNIData;

  public:
    template <class Archive> inline void serialize(Archive& ar, unsigned int const version)
    {
      ar & NRT_VAR( itsOpenNIData );
    }
};

float DepthImageMetadataMessage::getDepthFocalLength() const
{
	if( itsOpenNIData.valid )
		return itsOpenNIData.Depth.focalLength;
	else
		return -1.0f;
}

#endif // MESSAGES_IMAGEPROC_DEPTHIMAGEMETADATAMESSAGE_H

