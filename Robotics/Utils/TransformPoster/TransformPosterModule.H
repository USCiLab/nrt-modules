#ifndef TRANFORMVISUALIZERMODULE_H 
#define TRANFORMVISUALIZERMODULE_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>

namespace transformposter 
{
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformUpdate, nrt::TransformMessage, void, "The transform port");

  nrt::ParameterCategory ParamCateg("Transform Poster Related Options");
  nrt::ParameterDef<std::string> FromParamDef("from", "The base transform", "world", ParamCateg);
  nrt::ParameterDef<std::string> ToParamDef("to", "The end transform", "robot", ParamCateg);
  nrt::ParameterDef<std::string> TransformParamDef("transform", "The transform as a comma separated list (tx,ty,tz,rx,ry,rz)",
      "0,0,0,0,0,0", ParamCateg);
  nrt::ParameterDef<double> RateParamDef("rate", "The rate (in Hz) at which to repost the transform.", 0, ParamCateg);
}


class TransformPosterModule : public nrt::Module,
                              public nrt::MessagePoster<transformposter::TransformUpdate>
{
  public:
    TransformPosterModule(std::string const& instanceName="");

    void run(); 

    void transformCallback(std::string const & str);

  private:
    std::mutex itsMtx;
    nrt::Transform3D itsTransform;
    nrt::Parameter<std::string> itsFromParam;
    nrt::Parameter<std::string> itsToParam;
    nrt::Parameter<std::string> itsTransformParam;
    nrt::Parameter<double> itsRateParam;
};

#endif // TRANFORMVISUALIZERMODULE_H 
