/*! @file Algorithms/PointCloud/Filter/PointCloudFilterType.H
    Abstract class for filtering components */

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

#ifndef ALGORITHMS_POINTCLOUD_FILTER_POINTCLOUDFILTERTYPE_H_
#define ALGORITHMS_POINTCLOUD_FILTER_POINTCLOUDFILTERTYPE_H_

#include <nrt/Core/Model/Component.H>
#include <nrt/Core/Design/Factory.H>
#include <nrt/PointCloud/GenericCloud.H>
#include <nrt/Core/Typing/Enum.H>

NRT_MACRO_DEFINE_ENUM_CLASS(PointCloudFilterField, (geometry) (normals) (color));

namespace pointcloud_filters_type
{
  static nrt::ParameterCategory Options("Filter parameters");

  static nrt::ParameterDef<std::string> Field( "Filter field", "The specific data field that you wish to filter",
      "geometry", PointCloudFilterField_Strings, Options );

	static nrt::ParameterDef<bool> Preserve( "Preserve structure", "Whether to preserve the structure (ordering) of the point cloud."
			" Enabling this will usually result in a large loss of performance", false, Options );
}

class PointCloudFilterType : public nrt::Component
{
  public:
    PointCloudFilterType( std::string const & instanceID = "PointCloudSourceType" );

    //! Sets the input to the underlying filter algorithm
    virtual void setInput( nrt::GenericCloud const in ) = 0;

    //! Sets a subset of the input
		virtual void setIndices(nrt::DynamicArray<int> const indices ) = 0;

    //! Performs the filtering
		virtual nrt::GenericCloud filter() = 0;

	protected:
		PointCloudFilterField itsField;
		nrt::Parameter<bool> itsPreserveStructureParam;

	private:
    void fieldParamChanged( std::string const & newVal );
		nrt::Parameter<std::string> itsFieldParam;
};

typedef nrt::Factory<
PointCloudFilterType,
std::string,
std::function<std::shared_ptr<PointCloudFilterType>()>,
std::map<std::string, std::string>
> PointCloudFilterFactory;

/*! \def NRTILAB_REGISTER_POINTCLOUDFILTER(CLASSNAME)
  Register a PointCloudFilterType with a global Factory.
*/
#define NRTILAB_REGISTER_POINTCLOUDFILTER(CLASSNAME)                          \
    std::shared_ptr<PointCloudFilterType> create##CLASSNAME()            			\
    { return std::shared_ptr<PointCloudFilterType>(new CLASSNAME); }     			\
    bool pointCloudFilterRegister##CLASSNAME()                                \
    {                                                                         \
      std::map<std::string,std::string> metadata;                             \
      metadata["id"] = CLASSNAME::id();                                       \
      metadata["description"] = CLASSNAME::description();                     \
      PointCloudFilterFactory::instance().registerType(                  			\
          metadata["id"],                                                     \
          create##CLASSNAME,                                                  \
          metadata                                                            \
      );                                                                      \
      return true;                                                            \
    }                                                                         \
    const bool CLASSNAME##Registered __attribute__ ((unused)) =               \
                                        pointCloudFilterRegister##CLASSNAME();\
 
#endif // ALGORITHMS_POINTCLOUD_FILTER_POINTCLOUDSOURCETYPE_H_
