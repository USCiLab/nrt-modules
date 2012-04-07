/*! @file Algorithms/PointCloud/Filter/PointCloudFilter.C
    Base component class for filtering */

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

#include "PointCloudFilter.H"

PointCloudFilter::PointCloudFilter( std::string instanceID ) :
    nrt::Component( instanceID ),
    itsFilter( nullptr ),
    itsFilterTypeParam( createFilterTypeDef(), this, &PointCloudFilter::filterParamChanged )
{ }

nrt::ParameterDef<std::string> PointCloudFilter::createFilterTypeDef()
{
  std::string description = "The following are valid filter types:\n";
  std::vector<std::string> validSourceTypes = PointCloudFilterFactory::instance().getIDs();

	for ( std::string const & sourceType : validSourceTypes )
  {
    std::map<std::string, std::string> metadata = PointCloudFilterFactory::instance().getMetaData( sourceType );
    description += "    --filter=" + metadata["id"] + "\n";
    description += "             " + metadata["description"] + "\n";
  }

  return nrt::ParameterDef<std::string>( "filter", description, "passthrough", validSourceTypes, pointcloud_filter::FilterOptions );
}

void PointCloudFilter::setInput( nrt::GenericCloud const in )
{
  if (itsFilter == std::shared_ptr<PointCloudFilterType>())
    NRT_WARNING("No PointCloudFilter type choosen");
  else
		itsFilter->setInput( in );
}

void PointCloudFilter::setIndices( nrt::DynamicArray<int> const indices )
{
  if (itsFilter == std::shared_ptr<PointCloudFilterType>())
    NRT_WARNING("No PointCloudFilter type choosen");
  else
		itsFilter->setIndices( indices );
}

nrt::GenericCloud PointCloudFilter::filter()
{
  if (itsFilter == std::shared_ptr<PointCloudFilterType>())
	{
    NRT_WARNING("No PointCloudFilter type choosen");
		return nrt::GenericCloud();
	}
  else
		return itsFilter->filter();
}

void PointCloudFilter::filterParamChanged(std::string const & newtype)
{
	if( !PointCloudFilterFactory::instance().isRegistered( newtype ) )
	{
		std::string validTypes;
		std::vector<std::string> validSourceTypes = PointCloudFilterFactory::instance().getIDs();

		for( size_t i = 0; i < validSourceTypes.size(); ++i )
		{
			validTypes += validSourceTypes[i];
			if( i < validSourceTypes.size() - 1 )
				validTypes += ", ";
		}

		throw nrt::exception::BadParameter( "Unsupported PointCloudFilter type.  Valid types are [" + validTypes + "]" );
	}

	if( itsFilter )
		removeSubComponent( itsFilter );

	itsFilter = PointCloudFilterFactory::instance().create( newtype );
	addSubComponent( itsFilter );
}

std::shared_ptr<PointCloudFilterType> PointCloudFilter::actualFilter() const
{
  return itsFilter;
}
