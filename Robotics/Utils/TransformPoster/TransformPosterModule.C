#include "TransformPosterModule.H"
#include <nrt/Core/Util/StringUtil.H>

using namespace nrt;
using namespace transformposter;


// ######################################################################
TransformPosterModule::TransformPosterModule(std::string const & instanceName) :
  Module(instanceName),
  itsFromParam(FromParamDef, this),
  itsToParam(ToParamDef, this),
  itsTransformParam(TransformParamDef, this, &TransformPosterModule::transformCallback)
{ }

// ######################################################################
void TransformPosterModule::transformCallback(std::string const & str)
{
  std::lock_guard<std::mutex> _(itsMtx);

  std::vector<std::string> split = nrt::splitString(itsTransformParam.getVal(), ',');
  if(split.size() != 6) throw nrt::exception::BadParameter("Format should be: 'tx,ty,tz,rx,ry,rz'");

  itsTransform = 
    Eigen::Translation3d(
      boost::lexical_cast<double>(split[0]),
      boost::lexical_cast<double>(split[1]),
      boost::lexical_cast<double>(split[2]));

  itsTransform *= 
    Eigen::AngleAxisd(boost::lexical_cast<double>(split[3]), Eigen::Vector3d::UnitX()) * 
    Eigen::AngleAxisd(boost::lexical_cast<double>(split[4]), Eigen::Vector3d::UnitY()) * 
    Eigen::AngleAxisd(boost::lexical_cast<double>(split[5]), Eigen::Vector3d::UnitZ()); 
}

// ######################################################################
void TransformPosterModule::run()
{
  while(running())
  {
    std::unique_ptr<nrt::TransformMessage> msg(
        new nrt::TransformMessage(nrt::now(), itsFromParam.getVal(), itsToParam.getVal(), itsTransform));
    post<TransformUpdate>(msg);
  }
}

NRT_REGISTER_MODULE(TransformPosterModule);
