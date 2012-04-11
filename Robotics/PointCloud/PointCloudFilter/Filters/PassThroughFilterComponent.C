/*! @file Algorithms/PointCloud/Filter/PassthroughFilterComponent.C
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

#include "PassThroughFilterComponent.H"

// register with the factory
NRTILAB_REGISTER_POINTCLOUDFILTER( PassthroughFilterComponent );

PassthroughFilterComponent::PassthroughFilterComponent( std::string const & instanceId ) :
  PointCloudFilterType( instanceId ),
  itsIndex( pointcloud_filters_passthrough::Index, this, &PassthroughFilterComponent::indexCallback ),
  itsInvert( pointcloud_filters_passthrough::Invert, this, &PassthroughFilterComponent::invertCallback ),
  itsUpperBound( pointcloud_filters_passthrough::Upper, this, &PassthroughFilterComponent::upperCallback ),
  itsLowerBound( pointcloud_filters_passthrough::Lower, this, &PassthroughFilterComponent::lowerCallback ),
  itsReplace( pointcloud_filters_passthrough::Replace, this, &PassthroughFilterComponent::replaceCallback ),
  itsReplaceValue( pointcloud_filters_passthrough::ReplaceValue, this, &PassthroughFilterComponent::replaceValueCallback )
{ }

void PassthroughFilterComponent::setInput( nrt::GenericCloud const in )
{
  itsFilter.setInput( in.get<nrt::GenericPoint>() );
}

void PassthroughFilterComponent::setIndices( nrt::DynamicArray<int> const indices )
{
	itsFilter.setIndices( indices );
}

void PassthroughFilterComponent::indexCallback( nrt::int32 const & index )
{
  itsFilter.setFilterField( index );
}

void PassthroughFilterComponent::invertCallback( bool const & invert )
{
  itsFilter.setInvertFilter( invert );
}

void PassthroughFilterComponent::upperCallback( double const & upper )
{
  itsFilter.setUpperBound( upper );
}

void PassthroughFilterComponent::lowerCallback( double const & lower )
{
  itsFilter.setLowerBound( lower );
}

void PassthroughFilterComponent::replaceCallback( bool const & replace )
{
  itsFilter.setUseReplaceValue( replace );
}

void PassthroughFilterComponent::replaceValueCallback( double const & value )
{
  itsFilter.setReplaceValue( value );
}

nrt::GenericCloud PassthroughFilterComponent::filter()
{
  typedef nrt::PointDescriptorType<nrt::GenericPoint>::type Descriptor;
  typedef Descriptor::Geometry Geometry;
  typedef Descriptor::Normals Normals;
  typedef Descriptor::Color Color;

  nrt::PointCloud<nrt::GenericPoint> output;

  switch( itsField )
  {
    case PointCloudFilterField::geometry:
      itsFilter.filter<Geometry>( output );
      break;
    case PointCloudFilterField::normals:
      itsFilter.filter<Normals>( output );
      break;
    case PointCloudFilterField::color:
      itsFilter.filter<Color>( output );
      break;
    default:
      break;
  }

  return nrt::GenericCloud( output );
}
