#ifndef TRANFORMVISUALIZERMODULE_H 
#define TRANFORMVISUALIZERMODULE_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Core/Util/VectorUtils.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <cairo.h>
#include <gtk/gtk.h>

namespace transformvisualizer 
{
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformLookup, nrt::TransformLookupMessage, nrt::TransformMessage, "The transform port");

  nrt::ParameterCategory ParamCateg("Transform Visualizer Related Options");
  nrt::ParameterDef<std::string> WorldParamDef("world", "The base transform to use as an origin", "world", ParamCateg);
  nrt::ParameterDef<std::string> TransformsParamDef("transforms", "A comma-separated list of transforms to display",  "robot", ParamCateg);
  nrt::ParameterDef<double> ScaleParamDef("scale", "The display scale (in pixels/meter)",  10, ParamCateg);
}


class TransformVisualizerModule : public nrt::Module,
                                  public nrt::MessagePoster<transformvisualizer::TransformLookup>
{
  public:
    TransformVisualizerModule(std::string const& instanceName="");

    void run(); 

    std::map<std::string, nrt::Transform3D> getTransforms();

    void scaleChangedCallback(double const & scale);

  private:
    friend gboolean transform_canvas_expose(GtkWidget * canvas, GdkEventExpose * event, gpointer visualizerPtr);

    std::mutex itsMtx;
    void gtkThreadMethod();
    std::thread itsGtkThread;

    GtkWidget * itsWindow;
    GtkWidget * itsCanvas;

    std::map<std::string, nrt::Transform3D> itsTransforms;

    nrt::Parameter<std::string> itsWorldParam;
    nrt::Parameter<std::string> itsTransformsParam;
    nrt::Parameter<double> itsScaleParam;
};

#endif // TRANFORMVISUALIZERMODULE_H 
