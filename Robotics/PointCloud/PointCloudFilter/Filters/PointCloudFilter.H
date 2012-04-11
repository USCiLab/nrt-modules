/*! @file Algorithms/PointCloud/Filter/PointCloudFilter.H
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

#ifndef ALGORITHMS_POINTCLOUD_FILTER_POINTCLOUDFILTER_H_
#define ALGORITHMS_POINTCLOUD_FILTER_POINTCLOUDFILTER_H_

#include <nrt/Core/Model/Parameter.H>
#include "PointCloudFilterType.H"

namespace pointcloud_filter
{
  static nrt::ParameterCategory FilterOptions("Filter related parameters");
}

class PointCloudFilter : public nrt::Component
{
  public:
    PointCloudFilter(std::string instanceID="filter");
    
		//! Sets the input to the underlying filter algorithm
    virtual void setInput( nrt::GenericCloud const in );

    //! Sets indices subset
    virtual void setIndices( nrt::DynamicArray<int> const indices );

    //! Performs the filtering
		nrt::GenericCloud filter();

    //! Get a reference the the underlying implementation that was created
    //! from the --filter parameter
    std::shared_ptr<PointCloudFilterType> actualFilter() const;

  private:
    std::shared_ptr<PointCloudFilterType> itsFilter;
		nrt::Parameter<std::string> itsFilterTypeParam;

    nrt::ParameterDef<std::string> createFilterTypeDef();
    void filterParamChanged( std::string const & newVal );
};

#endif // ALGORITHMS_POINTCLOUD_FILTER_POINTCLOUDFILTER_H_
