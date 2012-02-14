#include "TransformVisualizerModule.H"

using namespace nrt;

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

static gboolean transform_canvas_expose(GtkWidget * canvas, GdkEventExpose * event)
{

  // Get a cairo_t
  cairo_t * cr = gdk_cairo_create(canvas->window);

  /* set a clip region for the expose event */
  cairo_rectangle (cr,
      event->area.x, event->area.y,
      event->area.width, event->area.height);
  cairo_clip (cr);

  cairo_destroy(cr);

  return FALSE;
}

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
  Module(instanceName)
{ 
  itsGtkThread = std::thread(std::bind(&TransformVisualizerModule::gtkThreadMethod, this));
}

void TransformVisualizerModule::postInit()
{
}

void TransformVisualizerModule::gtkThreadMethod()
{
  int fakeargs = 0;
  gtk_init(&fakeargs, NULL);
  itsWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  itsCanvas = transform_canvas_new();
  gtk_container_add(GTK_CONTAINER(itsWindow), itsCanvas);
  g_signal_connect(itsWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_widget_show_all(itsWindow);
  gtk_main();
}

// ######################################################################
void TransformVisualizerModule::run()
{
}

NRT_REGISTER_MODULE(TransformVisualizerModule);
