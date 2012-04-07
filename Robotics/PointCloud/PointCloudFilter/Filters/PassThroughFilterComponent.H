/*! @file Algorithms/PointCloud/Filter/PassthroughFilterComponent.H
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

#ifndef ALGORITHMS_POINTCLOUD_FILTER_PASSTHROUGHFILTERCOMPONENT_H_
#define ALGORITHMS_POINTCLOUD_FILTER_PASSTHROUGHFILTERCOMPONENT_H_

#include "PointCloudFilterType.H"
#include <nrt/PointCloud/Filter/PassThroughFilter.H>
#include <nrt/Core/Model/Parameter.H>

namespace pointcloud_filters_passthrough
{
  static nrt::ParameterCategory Options("Passthrough filter related parameters");

	static nrt::ParameterDef<nrt::int32> Index( "index in field to filter", 
			"Specifies the zero based index in the selected field to use to decide which points we filter, or -1 to do nothing.  For example, if filtering geometry, X is at 0, and in color, " 
			"r would be 0.", -1, Options );

	static nrt::ParameterDef<bool> Invert( "invert result", "Whether to filter in bounds or out of bounds (invert)", false, Options );

	static nrt::ParameterDef<double> Upper( "upper bound", "Upper bound on filtered values", std::numeric_limits<double>::max(), Options );
	static nrt::ParameterDef<double> Lower( "lower bound", "Lower bound on filtered values", -std::numeric_limits<double>::max(), Options );
	
	static nrt::ParameterDef<bool> Replace( "replace values", "Whether to replace filtered values instead of deleting them", false, Options );
	static nrt::ParameterDef<double> ReplaceValue( "replacement value", "Value used if replace is set to true", 0, Options );
}

//! A component that wraps the passthrough filter with parameters
class PassthroughFilterComponent : public PointCloudFilterType
{
  public:
    PassthroughFilterComponent( std::string const & instanceID = "passthrough filter" );

    //! Sets the input to the underlying filter algorithm
    void setInput( nrt::GenericCloud const in );

    //! Sets the indices (subset) of the current input
    void setIndices( nrt::DynamicArray<int> const indices );

    //! Performs the filtering
    nrt::GenericCloud filter();

		static std::string const id() { return "passthrough"; }

		static std::string const description() { return "Filters by passing through a subset of points"; }

  protected:
		void indexCallback( nrt::int32 const & index );
		void invertCallback( bool const & invert );
		void upperCallback( double const & upper );
		void lowerCallback( double const & lower );
		void replaceCallback( bool const & replace );
		void replaceValueCallback( double const & value );

  private:
    nrt::PassThroughFilter<nrt::GenericPoint> itsFilter;
		nrt::Parameter<nrt::int32> itsIndex;
		nrt::Parameter<bool> itsInvert;
		nrt::Parameter<double> itsUpperBound;
		nrt::Parameter<double> itsLowerBound;
		nrt::Parameter<bool> itsReplace;
		nrt::Parameter<double> itsReplaceValue;
};

#endif // ALGORITHMS_POINTCLOUD_FILTER_PASSTHROUGHFILTERCOMPONENT_H_
