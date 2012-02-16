#include "TransformPosterModule.H"
#include <nrt/Core/Util/StringUtil.H>

using namespace nrt;
using namespace transformposter;


// ######################################################################
TransformPosterModule::TransformPosterModule(std::string const & instanceName) :
  Module(instanceName),
  itsFromParam(FromParamDef, this),
  itsToParam(ToParamDef, this),
  itsTransformParam(TransformParamDef, this, &TransformPosterModule::transformCallback),
  itsRateParam(RateParamDef, this)
{ }

// ######################################################################
void TransformPosterModule::transformCallback(std::string const & str)
{
  if(!initialized()) return;

  std::lock_guard<std::mutex> _(itsMtx);

  std::vector<std::string> split = nrt::splitString(str, ',');
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

  Eigen::Translation3d translation(
      boost::lexical_cast<double>(split[0]),
      boost::lexical_cast<double>(split[1]),
      boost::lexical_cast<double>(split[2]));

  // Post the newly changed transform
  if(!initialized()) return;

  postTransform();
}

// ######################################################################
void TransformPosterModule::postTransform()
{
  std::unique_ptr<nrt::TransformMessage> msg(
      new nrt::TransformMessage(nrt::now(), itsFromParam.getVal(), itsToParam.getVal(), itsTransform));
  post<TransformUpdate>(msg);
}

// ######################################################################
void TransformPosterModule::run()
{

  nrt::Timer timer;

  // Always post a transform as soon as we're started
  { std::lock_guard<std::mutex> _(itsMtx); postTransform(); }

  while(running())
  {
    timer.start();
    int const ratetime_us = 1000000.0 / itsRateParam.getVal();
    if(itsRateParam.getVal() == 0)
    {
      sleep(1); 
      continue;
    }

    // Post the transform
    { std::lock_guard<std::mutex> _(itsMtx); postTransform(); }

    // Obey our rate
    int const time_us = timer.getDuration() * 1000000.0;
    if(ratetime_us - time_us > 0) usleep(ratetime_us - time_us);
  }
}

NRT_REGISTER_MODULE(TransformPosterModule);
