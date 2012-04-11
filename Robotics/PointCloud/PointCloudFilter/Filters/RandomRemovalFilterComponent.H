/*! @file Algorithms/PointCloud/Filter/RandomRemovalFilterComponent.H
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

#ifndef ALGORITHMS_POINTCLOUD_FILTER_RANDOMREMOVALFILTERCOMPONENT_H_
#define ALGORITHMS_POINTCLOUD_FILTER_RANDOMREMOVALFILTERCOMPONENT_H_

#include "PointCloudFilterType.H"
#include <nrt/PointCloud/Filter/RandomRemovalFilter.H>
#include <nrt/Core/Model/Parameter.H>

namespace pointcloud_filters_random
{
  static nrt::ParameterCategory Options("Random removal filter related parameters");

	static nrt::ParameterDef<double> Removal( "removal percentage", "Percentage of points to be removed (0.0 to 1.0)", 0.0, Options );
}

//! A component that wraps the passthrough filter with parameters
class RandomRemovalFilterComponent : public PointCloudFilterType
{
  public:
    RandomRemovalFilterComponent( std::string const & instanceID = "passthrough filter" );

    //! Sets the input to the underlying filter algorithm
    void setInput( nrt::GenericCloud const in );

    //! Sets the indices (subset) of the current input
    void setIndices( nrt::DynamicArray<int> const indices );

    //! Performs the filtering
    nrt::GenericCloud filter();

		static std::string const id() { return "random"; }

		static std::string const description() { return "Filters by randomly removing points"; }

  protected:
		void removalCallback( double const & percent );

  private:
    nrt::RandomRemovalFilter<nrt::GenericPoint> itsFilter;
		nrt::Parameter<double> itsRemovalPercent; 
};

#endif // ALGORITHMS_POINTCLOUD_FILTER_RandomRemovalFilterComponent_H_
