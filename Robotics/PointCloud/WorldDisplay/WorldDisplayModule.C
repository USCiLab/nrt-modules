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
// Primary maintainer for this file: Randolph Voorhies <voorhies@usc.edu>
//

#include "WorldDisplayModule.H"

using namespace worlddisplay;
using namespace nrt;

// ######################################################################
WorldDisplayModule::WorldDisplayModule(std::string const & instanceid) :
    Module(instanceid),
    itsWindow("NRT World View"),
    itsBaseParam(baseParamDef, this),
    itsTransformsParam(transformsParamDef, this, &WorldDisplayModule::transformParamCallback),
    itsFramerateParam(framerateParamDef, this)
{ 
  itsWindow.setUserDraw(std::bind(&WorldDisplayModule::drawFrames, this));
  itsWindow.setDrawAxes(true);
  itsWindow.setDrawPlane(GLWindowPointCloud::Plane::xy);
}

// ######################################################################
void WorldDisplayModule::transformParamCallback(std::string const & transforms)
{
  std::lock_guard<std::mutex> _(itsMtx);
  itsTransformNames = splitString(transforms, ',');
  for(std::string & transform : itsTransformNames)
    transform = trimString(transform);
}

// ######################################################################
void WorldDisplayModule::drawFrames()
{

  std::map<std::string, nrt::Transform3D> transforms;
  {
    std::lock_guard<std::mutex> _(itsMtx);
    transforms = itsTransforms;
  }

  for(std::pair<std::string, Transform3D> const & tran : transforms)
  {
    const float len = 0.1f;
    Eigen::Vector3f centervec = (tran.second * Eigen::Vector3d(0,0,0)).cast<float>();
    Eigen::Vector3f xvec = (tran.second * Eigen::Vector3d(len, 0, 0)).cast<float>();
    Eigen::Vector3f yvec = (tran.second * Eigen::Vector3d(0, len, 0)).cast<float>();
    Eigen::Vector3f zvec = (tran.second * Eigen::Vector3d(0, 0, len)).cast<float>();

    GLfloat axesOrigin[] = {centervec.x(), centervec.y(), centervec.z()};
    GLfloat axesX[] = {xvec.x(), xvec.y(), xvec.z()};
    GLfloat axesY[] = {yvec.x(), yvec.y(), yvec.z()};
    GLfloat axesZ[] = {zvec.x(), zvec.y(), zvec.z()};

    glPushAttrib(GL_CURRENT_BIT);
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3ub(255,0,0);
    glVertex3fv(axesOrigin);
    glVertex3fv(axesX);
    glColor3ub(0,255,0);
    glVertex3fv(axesOrigin);
    glVertex3fv(axesY);
    glColor3ub(0,0,255);
    glVertex3fv(axesOrigin);
    glVertex3fv(axesZ);
    glEnd();
    glPopAttrib();
    glPopAttrib();

    GLfloat textColor[] = {1, 1, 1};
    itsWindow.drawText(tran.first, textColor, axesZ);
  }
}

// ######################################################################
void WorldDisplayModule::run()
{
  itsWindow.show();
  while(running())
  {
    auto waitTime = now() + std::chrono::milliseconds((int)std::round(1000.0/itsFramerateParam.getVal()));

    std::map<std::string, Transform3D> transforms;
    std::string const & baseName = itsBaseParam.getVal();
    for(std::string const& transformName : itsTransformNames)
    {
      try
      {
        // Lookup the specified transform
        auto lookupMessage = make_unique( new TransformLookupMessage( now(), baseName, transformName ) );
        MessagePosterResults<TransformLookup> results = post<TransformLookup>(lookupMessage);
        if(results.size() == 0) { NRT_WARNING("No TransformManagers detected");        sleep(1); break; }
        if(results.size() > 1)  { NRT_WARNING("Multiple TransformManagers detected!"); sleep(1); break; }
        TransformMessage::const_ptr transform = results.get();
        {
          transforms[transformName] = transform->transform;
        }
      }
      catch(...)
      {
        NRT_WARNING("Error looking up transform from [" << baseName << "] to [" << transformName << "]");
      }
    }

    {
      std::lock_guard<std::mutex> _(itsMtx);
      itsTransforms = transforms;
    }

    if( auto res = check<Cloud>(nrt::MessageCheckerPolicy::Unseen) )
    {
      itsWindow.setCloud(res.get()->cloud, true);
      itsWindow.show();
    }

    if(nrt::now() > waitTime) NRT_WARNING("Cannot keep up framerate!");

    std::this_thread::sleep_until(waitTime);
  }

}

NRT_REGISTER_MODULE(WorldDisplayModule);
