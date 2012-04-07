/*! @file Algorithms/PointCloud/Filter/StatisticalOutlierFilterComponent.H
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

#ifndef ALGORITHMS_POINTCLOUD_FILTER_STATISTICALOUTLIERFILTERCOMPONENT_H_
#define ALGORITHMS_POINTCLOUD_FILTER_STATISTICALOUTLIERFILTERCOMPONENT_H_

#include "PointCloudFilterType.H"
#include <nrt/PointCloud/Filter/StatisticalOutlierFilter.H>
#include <nrt/Core/Model/Parameter.H>

namespace pointcloud_filters_stat
{
  static nrt::ParameterCategory Options("Random removal filter related parameters");

	static nrt::ParameterDef<nrt::int32> K( "neighbors", "Number of neighbors to look at", 2, Options );
	static nrt::ParameterDef<double> Std( "std multiplier", "Multiplies the standard deviation of local statistics", 0.0, Options );
	static nrt::ParameterDef<bool> Invert( "invert", "Whether we should invert (discard inliers)", false, Options );
}

//! A component that wraps the passthrough filter with parameters
class StatisticalOutlierFilterComponent : public PointCloudFilterType
{
  public:
    StatisticalOutlierFilterComponent( std::string const & instanceID = "statistical outlier removal filter" );

    //! Sets the input to the underlying filter algorithm
    void setInput( nrt::GenericCloud const in );

    //! Sets the indices (subset) of the current input
    void setIndices( nrt::DynamicArray<int> const indices );

    //! Performs the filtering
    nrt::GenericCloud filter();

		static std::string const id() { return "statistical"; }

		static std::string const description() { return "Filters by removing points based on local statistics"; }

  protected:
		void kCallback( nrt::int32 const & k );
		void stdCallback( double const & std );
		void invertCallback( bool const & invert );

  private:
    nrt::StatisticalOutlierFilter<nrt::GenericPoint> itsFilter;
		nrt::Parameter<nrt::int32> itsK; 
		nrt::Parameter<double> itsStd; 
		nrt::Parameter<bool> itsInvert; 
};

#endif // ALGORITHMS_POINTCLOUD_FILTER_STATISTICALOUTLIERFILTERCOMPONENT_H_
