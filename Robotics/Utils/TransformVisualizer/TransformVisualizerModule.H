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
  nrt::ParameterDef<std::string> ToParamDef("to",     "The end transform",  "robot", ParamCateg);
  nrt::ParameterDef<double> ScaleParamDef("scale", "The display scale (in pixels/meter)",  10, ParamCateg);
}


class TransformVisualizerModule : public nrt::Module,
                                  public nrt::MessagePoster<transformvisualizer::TransformLookup>
{
  public:
    TransformVisualizerModule(std::string const& instanceName="");

    void run(); 

    nrt::Transform3D getTransform();

    void scaleChangedCallback(double const & scale);

  private:
    friend gboolean transform_canvas_expose(GtkWidget * canvas, GdkEventExpose * event, gpointer visualizerPtr);

    std::mutex itsMtx;
    void gtkThreadMethod();
    std::thread itsGtkThread;

    GtkWidget * itsWindow;
    GtkWidget * itsCanvas;

    nrt::Transform3D itsTransform;

    nrt::Parameter<std::string> itsFromParam;
    nrt::Parameter<std::string> itsToParam;
    nrt::Parameter<double> itsScaleParam;
};

#endif // TRANFORMVISUALIZERMODULE_H 
