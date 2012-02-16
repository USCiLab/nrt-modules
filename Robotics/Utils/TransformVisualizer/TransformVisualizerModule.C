#include "TransformVisualizerModule.H"

using namespace nrt;
using namespace transformvisualizer;

// ######################################################################
void drawFrame(GtkWidget * canvas, cairo_t * cr, nrt::Transform3D const & transform, double const scale, std::string name)
{
  double const center_x = canvas->allocation.x + canvas->allocation.width / 2;
  double const center_y = canvas->allocation.y + canvas->allocation.height / 2;

  double const radius = 5;

  // Set the transform into window coordinates [(0,0) at the center of the screen]
  nrt::Transform3D scaledTransform = transform; scaledTransform.prescale(scale);
  nrt::Transform3D windowTransform = Eigen::Translation3d(center_x, center_y, 0) * scaledTransform;

  // Transform the origin, and a little direction arrow into the robot/window coordinate frame
  Eigen::Vector3d frameCenter = windowTransform * Eigen::Vector3d(0, 0, 0);
  Eigen::Vector3d frameArrow  = windowTransform * Eigen::Vector3d(radius*2/scale, 0, 0);

  // Draw the target frame
  cairo_set_source_rgba (cr, 0.0, 0.9, 0.0, 0.9);
  cairo_arc(cr, frameCenter.x(), frameCenter.y(), radius, 0, 2 * M_PI);
  cairo_fill(cr);
  cairo_move_to(cr, frameCenter.x(),frameCenter.y());
  cairo_line_to(cr, frameArrow.x(), frameArrow.y());
  cairo_stroke (cr);

  // Write the frame's name
  cairo_set_source_rgba(cr, 1, 1, 1, 0.3); 
  cairo_move_to(cr, frameCenter.x() + radius*1.5, frameCenter.y());
  cairo_show_text(cr, name.c_str());
}

// ######################################################################
gboolean 
transform_canvas_expose(GtkWidget * canvas, GdkEventExpose * event, gpointer visualizerPtr)
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

  double const center_x = canvas->allocation.x + canvas->allocation.width / 2;
  double const center_y = canvas->allocation.y + canvas->allocation.height / 2;
  double const left     = canvas->allocation.x;
  double const right    = canvas->allocation.x + canvas->allocation.width;
  double const top      = canvas->allocation.y;
  double const bottom   = canvas->allocation.y + canvas->allocation.height;

  // Draw the axes
  cairo_set_source_rgba(cr, 1, 1, 1, 0.3);
  cairo_move_to(cr, center_x, top);
  cairo_line_to(cr, center_x, bottom);
  cairo_stroke(cr);
  cairo_move_to(cr, left, center_y);
  cairo_line_to(cr, right, center_y);
  cairo_stroke(cr);


  TransformVisualizerModule * visualizer = static_cast<TransformVisualizerModule*>(visualizerPtr);
  double const scale = visualizer->itsScaleParam.getVal();

  // Draw each of the target frames
  for(std::pair<std::string, nrt::Transform3D> const & transform : visualizer->getTransforms())
    drawFrame(canvas, cr, transform.second, scale, transform.first);

  cairo_destroy(cr);

  return FALSE;
}

// ######################################################################
TransformVisualizerModule::TransformVisualizerModule(std::string const & instanceName) :
  Module(instanceName),
  itsWorldParam(WorldParamDef, this),
  itsTransformsParam(TransformsParamDef, this),
  itsScaleParam(ScaleParamDef, this, &TransformVisualizerModule::scaleChangedCallback)
{ }

// ######################################################################
void TransformVisualizerModule::scaleChangedCallback(double const & scale)
{
  if(scale <= 0) throw nrt::exception::BadParameter("Scale must be > 0");
}

// ######################################################################
void TransformVisualizerModule::gtkThreadMethod()
{
  g_thread_init(NULL);
  gdk_threads_init();
  gdk_threads_enter();
  int fakeargs = 0;
  gtk_init(&fakeargs, NULL);
  itsWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  itsCanvas = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(itsWindow), itsCanvas);
  g_signal_connect(itsCanvas, "expose-event", G_CALLBACK(transform_canvas_expose), this);
  gtk_window_set_title(GTK_WINDOW(itsWindow), "Transform Visualizer");
  gtk_widget_show_all(itsWindow);
  gtk_main();
  gdk_threads_leave();
}

// ######################################################################
std::map<std::string, Transform3D> TransformVisualizerModule::getTransforms()
{
  std::lock_guard<std::mutex> _(itsMtx);
  return itsTransforms;
}

// ######################################################################
void TransformVisualizerModule::run()
{
  itsGtkThread = std::thread(std::bind(&TransformVisualizerModule::gtkThreadMethod, this));

  while(running())
  {
    usleep(10000);

    itsTransforms.clear();

    for(std::string const& transformName : nrt::splitString(itsTransformsParam.getVal(), ','))
    {
      try
      {
        // Lookup the specified transform
        auto lookupMessage = nrt::make_unique( new TransformLookupMessage( nrt::now(), itsWorldParam.getVal(), transformName ) );
        MessagePosterResults<TransformLookup> results = post<TransformLookup>(lookupMessage);
        if(results.size() == 0) { NRT_WARNING("No TransformManagers detected");        sleep(1); break;}
        if(results.size() > 1)  { NRT_WARNING("Multiple TransformManagers detected!"); sleep(1); break;}
        TransformMessage::const_ptr transform = results.get();
        {
          std::lock_guard<std::mutex> _(itsMtx);
          itsTransforms[transformName] = transform->transform;
        }
      }
      catch(...)
      {
        NRT_WARNING("Error looking up transform from [" << itsWorldParam.getVal() << "] to [" << transformName << "]");
        sleep(1);
      }

    }

    gdk_threads_enter();
    gtk_widget_draw(GTK_WIDGET(itsWindow), NULL);
    gdk_threads_leave();
  }

  gdk_threads_enter();
  gtk_widget_destroy(itsCanvas);
  gtk_widget_destroy(itsWindow);
  gtk_main_quit();
  gdk_threads_leave();
  itsGtkThread.join();
}

NRT_REGISTER_MODULE(TransformVisualizerModule);

