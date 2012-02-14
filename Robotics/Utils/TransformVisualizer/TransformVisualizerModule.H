#ifndef TRANFORMVISUALIZERMODULE_H 
#define TRANFORMVISUALIZERMODULE_H 

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformLookupMessage.H>
#include <cairo.h>
#include <gtk/gtk.h>

namespace transformvisualizer 
{
  NRT_DECLARE_MESSAGEPOSTER_PORT(TransformLookup, 
      nrt::TransformLookupMessage, void, "The transform port");
}


class TransformVisualizerModule : public nrt::Module,
                                  public nrt::MessagePoster<transformvisualizer::TransformLookup>
{
  public:
    TransformVisualizerModule(std::string const& instanceName="");

    void postInit();
    void run(); 

  private:
    void gtkThreadMethod();
    std::thread itsGtkThread;

    GtkWidget * itsWindow;
    GtkWidget * itsCanvas;
};

#endif // TRANFORMVISUALIZERMODULE_H 
