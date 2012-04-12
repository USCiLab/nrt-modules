/*! @file Modules/PointCloud/Common/Transform/TransformModule.C
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
#include "PointCloudTransformModule.H"
#include <nrt/PointCloud/Common/Transforms.H>

using namespace pointcloud_transform_module;

// ######################################################################
PointCloudTransformModule::PointCloudTransformModule( std::string const & instanceId ) :
  nrt::Module(instanceId),
  itsFromParam(fromParamDef, this),
  itsToParam(toParamDef, this)
{ }

// ######################################################################
void PointCloudTransformModule::onMessage( pointcloud_transform_module::Input in )
{
	// Apply the transform
	nrt::PointCloud<nrt::GenericPoint> cloud = in->cloud.get<nrt::GenericPoint>();
	nrt::PointCloud<nrt::GenericPoint> output;

  // Lookup the requested Transform
  Eigen::Matrix4f transform;
  try 
  {
    std::unique_ptr<nrt::TransformLookupMessage> lookupMessage(
        new nrt::TransformLookupMessage(nrt::now(), itsFromParam.getVal(), itsToParam.getVal()));
    nrt::MessagePosterResults<TransformLookup> results = post<TransformLookup>(lookupMessage);
    if(results.size() == 0) { throw NRT_MODULE_EXCEPTION("No TransformManagers detected"); }
    if(results.size() > 1)  { throw NRT_MODULE_EXCEPTION("Multiple TransformManagers detected!"); }

    transform = results.get()->transform.matrix().cast<float>();
  }
  catch(nrt::exception::ModuleException e)
  {
    NRT_WARNING(e.what());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return;
  }

  nrt::transformPointCloud( cloud, transform, output );

  // post the cloud
  std::unique_ptr<GenericCloudMessage> cloudMsg( new GenericCloudMessage( output ) );
  post<pointcloud_transform_module::Output>( cloudMsg );
}

// Don't forget this to be able to use your module as a runtime-loadable shared object
NRT_REGISTER_MODULE(PointCloudTransformModule);
