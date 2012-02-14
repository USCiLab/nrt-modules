#ifndef TRANFORMVISUALIZERMODULE_H 
#define TRANFORMVISUALIZERMODULE_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <cairo.h>
#include <gtk/gtk.h>

namespace transformvisualizer 
{
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformLookup, nrt::TransformLookupMessage, nrt::TransformMessage, "The transform port");

  nrt::ParameterCategory ParamCateg("Transform Visualizer Related Options");
  nrt::ParameterDef<std::string> FromParamDef("from", "The base transform", "world", ParamCateg);
  nrt::ParameterDef<std::string> ToParamDef("to",     "The end transform", "world", ParamCateg);
}


class TransformVisualizerModule : public nrt::Module,
                                  public nrt::MessagePoster<transformvisualizer::TransformLookup>
{
  public:
    TransformVisualizerModule(std::string const& instanceName="");

    void run(); 

  private:
    void gtkThreadMethod();
    std::thread itsGtkThread;

    GtkWidget * itsWindow;
    GtkWidget * itsCanvas;

    nrt::Parameter<std::string> itsFromParam;
    nrt::Parameter<std::string> itsToParam;
};

#endif // TRANFORMVISUALIZERMODULE_H 
