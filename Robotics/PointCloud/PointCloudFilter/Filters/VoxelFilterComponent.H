/*! @file Algorithms/PointCloud/Filter/VoxelFilterComponent.H
    A component for voxel filtering */

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

#ifndef ALGORITHMS_POINTCLOUD_FILTER_VOXELFILTERCOMPONENT_H_
#define ALGORITHMS_POINTCLOUD_FILTER_VOXELFILTERCOMPONENT_H_

#include "PointCloudFilterType.H"
#include <nrt/PointCloud/Filter/VoxelFilter.H>
#include <nrt/Core/Model/Parameter.H>

NRT_MACRO_DEFINE_ENUM_CLASS( VoxelFilterComponentDownFunc, (GeometryAverage) (GeometryColorAverage) );

namespace pointcloud_filters_voxel
{
  static nrt::ParameterCategory Options("Voxel filter related parameters");

  static nrt::ParameterDef<std::string> Down( "Downsampling function", "The downsampling function to apply to all points within a voxel",
      "GeometryAverage", VoxelFilterComponentDownFunc_Strings, Options );

  static nrt::ParameterDef<double> X( "voxel x", "Grid size for voxel filter in the x direction (in meters)", 0.0, Options );
  static nrt::ParameterDef<double> Y( "voxel y", "Grid size for voxel filter in the y direction (in meters)", 0.0, Options );
  static nrt::ParameterDef<double> Z( "voxel z", "Grid size for voxel filter in the z direction (in meters)", 0.0, Options );
}

//! A component that wraps the passthrough filter with parameters
class VoxelFilterComponent : public PointCloudFilterType
{
  public:
    VoxelFilterComponent( std::string const & instanceID = "voxel filter" );

    //! Sets the input to the underlying filter algorithm
    void setInput( nrt::GenericCloud const in );

    //! Sets the indices (subset) of the current input
    void setIndices( nrt::DynamicArray<int> const indices );

    //! Performs the filtering
    nrt::GenericCloud filter();

    static std::string const id() { return "voxel"; }

    static std::string const description() { return "Filters by binning points in a voxel grid"; }

  protected:
    void xCallback( double const & x );
    void yCallback( double const & y );
    void zCallback( double const & z );
    void downCallback( std::string const & down );

  private:
    nrt::VoxelFilter<nrt::GenericPoint> itsFilter;
    nrt::Parameter<std::string> itsDownString;
    nrt::Parameter<double> itsX;
    nrt::Parameter<double> itsY;
    nrt::Parameter<double> itsZ;
    VoxelFilterComponentDownFunc itsDownFunc;
};

#endif // ALGORITHMS_POINTCLOUD_FILTER_VOXELFILTERCOMPONENT_H_
