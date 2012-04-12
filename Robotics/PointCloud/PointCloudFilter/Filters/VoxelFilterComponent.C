/*! @file Algorithms/PointCloud/Filter/VoxelFilterComponent.C
    A component for radius filtering */

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

#include "VoxelFilterComponent.H"

// register with the factory
NRTILAB_REGISTER_POINTCLOUDFILTER( VoxelFilterComponent );

VoxelFilterComponent::VoxelFilterComponent( std::string const & instanceId ) :
  PointCloudFilterType( instanceId ),
  itsDownString( pointcloud_filters_voxel::Down, this, &VoxelFilterComponent::downCallback ),
  itsX( pointcloud_filters_voxel::X, this, &VoxelFilterComponent::xCallback ),
  itsY( pointcloud_filters_voxel::Y, this, &VoxelFilterComponent::yCallback ),
  itsZ( pointcloud_filters_voxel::Z, this, &VoxelFilterComponent::zCallback )
{ }

void VoxelFilterComponent::setInput( nrt::GenericCloud const in )
{
  itsFilter.setInput( in.get<nrt::GenericPoint>() );
}

void VoxelFilterComponent::setIndices( nrt::DynamicArray<int> const indices )
{
  itsFilter.setIndices( indices );
}

void VoxelFilterComponent::xCallback( double const & x )
{
  auto geo = itsFilter.getVoxelSize();
  geo[0] = x;
  itsFilter.setVoxelSize( geo );
}

void VoxelFilterComponent::yCallback( double const & y )
{
  auto geo = itsFilter.getVoxelSize();
  geo[1] = y;
  itsFilter.setVoxelSize( geo );
}

void VoxelFilterComponent::zCallback( double const & z )
{
  auto geo = itsFilter.getVoxelSize();
  geo[2] = z;
  itsFilter.setVoxelSize( geo );
}

void VoxelFilterComponent::downCallback( std::string const & down )
{
  try
  {
    std::istringstream is( down );
    is >> itsDownFunc;
  }
  catch( ... )
  {
    throw nrt::exception::BadParameter( "Unsupported field name" );
  }
}

nrt::GenericCloud VoxelFilterComponent::filter()
{
  nrt::PointCloud<nrt::GenericPoint> output;

  std::function<void( nrt::PointCloud<nrt::GenericPoint> const, nrt::VoxelFilter<nrt::GenericPoint>::Voxel const &, nrt::GenericPoint &)> voxelFunc;

  switch( itsDownFunc )
  {
    case VoxelFilterComponentDownFunc::GeometryAverage:
      voxelFunc = itsFilter.GeometryAverage<nrt::GenericPoint>;
      break;
    case VoxelFilterComponentDownFunc::GeometryColorAverage:
      voxelFunc = itsFilter.GeometryColorAverage<nrt::GenericPoint>;
      break;
    default:
      return output;
  }

	itsFilter.filter( output, voxelFunc );

  return nrt::GenericCloud( output );
}
