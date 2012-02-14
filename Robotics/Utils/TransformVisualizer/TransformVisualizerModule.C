#include "TransformVisualizerModule.H"

using namespace nrt;
using namespace transformvisualizer;

static gboolean transform_canvas_expose(GtkWidget * canvas, GdkEventExpose * event)
{

  // Get a cairo_t
  cairo_t * cr = gdk_cairo_create(canvas->window);

  /* set a clip region for the expose event */
  cairo_rectangle (cr,
      event->area.x, event->area.y,
      event->area.width, event->area.height);
  cairo_clip (cr);

  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_rectangle (cr,
      event->area.x, event->area.y,
      event->area.width, event->area.height);
  cairo_fill(cr);

  double center_x = canvas->allocation.x + canvas->allocation.width / 2;
  double center_y = canvas->allocation.y + canvas->allocation.height / 2;

  cairo_move_to(cr, center_x, center_y);
  cairo_line_to(cr, 1,10);
  cairo_set_source_rgb (cr, 0, 0, 255);
  cairo_stroke (cr);


  cairo_destroy(cr);

  return FALSE;
}


#define TRANSFORM_CANVAS_TYPE           (transform_canvas_get_type ())
#define TRANSFORM_CANVAS(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRANSFORM_CANVAS_TYPE, TransformCanvas))
#define TRANSFORM_CANVAS_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), TRANSFORM_CANVAS,  TransformCanvasClass))
#define IS_TRANSFORM_CANVAS(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRANSFORM_CANVAS_TYPE))
#define IS_TRANSFORM_CANVAS_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), TRANSFORM_CANVAS_TYPE))
#define TRANSFORM_CANVAS_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), TRANSFORM_CANVAS_TYPE, TransformCanvasClass))

struct _TransformCanvas
{
  GtkDrawingArea parent;
};

struct _TransformCanvasClass
{
  GtkDrawingAreaClass parent_class;
};

typedef _TransformCanvas         TransformCanvas;
typedef _TransformCanvasClass    TransformCanvasClass;
G_DEFINE_TYPE (TransformCanvas, transform_canvas, GTK_TYPE_DRAWING_AREA);

static void transform_canvas_class_init(TransformCanvasClass * canvas)
{
  GtkWidgetClass *widget_class;
  widget_class = GTK_WIDGET_CLASS(canvas);
  widget_class->expose_event = transform_canvas_expose;
}

static void transform_canvas_init(TransformCanvas *canvas)
{
}

GtkWidget * transform_canvas_new(void)
{
  return static_cast<GtkWidget*>(g_object_new(TRANSFORM_CANVAS_TYPE, NULL));
}

// ######################################################################
TransformVisualizerModule::TransformVisualizerModule(std::string const & instanceName) :
  Module(instanceName),
  itsFromParam(FromParamDef, this),
  itsToParam(ToParamDef, this)
{ 
}

void TransformVisualizerModule::gtkThreadMethod()
{
  g_thread_init(NULL);
  gdk_threads_init();
  gdk_threads_enter();
  int fakeargs = 0;
  gtk_init(&fakeargs, NULL);
  itsWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  itsCanvas = transform_canvas_new();
  gtk_container_add(GTK_CONTAINER(itsWindow), itsCanvas);
  g_signal_connect(itsWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_window_set_title(GTK_WINDOW(itsWindow), "Transform Visualizer");
  gtk_widget_show_all(itsWindow);
  gtk_main();
  gdk_threads_leave();
}

// ######################################################################
void TransformVisualizerModule::run()
{
  itsGtkThread = std::thread(std::bind(&TransformVisualizerModule::gtkThreadMethod, this));

  while(running())
  {
    usleep(10000);

    try
    {
      // Lookup the specified transform
      auto lookupMessage = nrt::make_unique( new TransformLookupMessage( nrt::now(), itsFromParam.getVal(), itsToParam.getVal() ) );
      MessagePosterResults<TransformLookup> results = post<TransformLookup>(lookupMessage);
      if(results.size() == 0) { NRT_WARNING("No TransformManagers detected");        sleep(1); continue;}
      if(results.size() > 1)  { NRT_WARNING("Multiple TransformManagers detected!"); sleep(1); continue;}
      TransformMessage::const_ptr transform = results.get();
    }
    catch(...)
    {
      NRT_WARNING("Error looking up transform from [" << itsFromParam.getVal() << "] to [" << itsToParam.getVal() << "]");
    }
  }
  
  gdk_threads_enter();
  gtk_widget_destroy(itsCanvas);
  gtk_widget_destroy(itsWindow);
  gtk_main_quit();
  gdk_threads_leave();
  itsGtkThread.join();
}

NRT_REGISTER_MODULE(TransformVisualizerModule);
