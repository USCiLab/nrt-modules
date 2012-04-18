#include "PointCloud2OccupancyModule.H"

using namespace pointcloud2occupancy;
using namespace nrt;

// ######################################################################
PointCloud2OccupancyModule::PointCloud2OccupancyModule(std::string const & instanceId) :
  Module(instanceId),
  itsLowerThreshParam(lowerThresholdParamDef, this),
  itsUpperThreshParam(upperThresholdParamDef, this),
  itsPixelsPerMeterParam(pixelsPerMeterParamDef, this),
  itsMapDimsParam(mapDimsParamDef, this),
  itsShadowsParam(shadowsParamDef, this)
{ }

// ######################################################################
struct OccupancyVisitor
{
  OccupancyVisitor(Image<PixGray<byte>, UniqueAccess> & map) :
    itsMap(map)
  { }

  template<class PointT, class Descriptor>
  void operator()(PointCloud<PointT, Descriptor> const& cloud) const
  {
    int cx = itsMap.width()/2;
    int cy = itsMap.height()/2;
    for(PointT const & point : cloud)
    {
      float const x = point.template get<Point3DEf>().x();
      float const y = point.template get<Point3DEf>().y();
      float const z = point.template get<Point3DEf>().z();

      if(z >= lowerThresh && z <= upperThresh)
      {
        int const px = x*ppm + cx;
        int const py = y*ppm + cx;
        if(itsMap.coordsOk(px, py)) itsMap(px, py) = 255;
      }
    }
  }

  Image<PixGray<byte>, UniqueAccess> & itsMap;

  float lowerThresh;
  float upperThresh;
  float ppm;
};

#define plot(x,y) \
  if(x < 0 || x >= w || y < 0 || y >= h) return; \
  byte val = img(x,y).val(); \
  if(val == 255) foundWall = true; \
  else if(foundWall) img(x,y) = PixGray<byte>(128);
void rasterLine(Image<PixGray<byte>, UniqueAccess> & img, int x2, int y2, byte val)
{
  NRT_INFO("  " << x2 << "," << y2);
  int w = img.width();
  int h = img.height();
  int x1 = w/2;
  int y1 = h/2;

  // if x1 == x2 or y1 == y2, then it does not matter what we set here
  int delta_x(x2 - x1);
  signed char ix((delta_x > 0) - (delta_x < 0));
  delta_x = std::abs(delta_x) << 1;

  int delta_y(y2 - y1);
  signed char iy((delta_y > 0) - (delta_y < 0));
  delta_y = std::abs(delta_y) << 1;

  bool foundWall = false;
  if (delta_x >= delta_y)
  {
    // error may go below zero
    int error(delta_y - (delta_x >> 1));

    while (x1 != x2)
    {
      if (error >= 0)
      {
        if (error || (ix > 0))
        {
          y1 += iy;
          error -= delta_x;
        }
        // else do nothing
      }
      // else do nothing

      x1 += ix;
      error += delta_y;

      plot(x1, y1);
    }
  }
  else
  {
    // error may go below zero
    int error(delta_x - (delta_y >> 1));

    while (y1 != y2)
    {
      if (error >= 0)
      {
        if (error || (iy > 0))
        {
          x1 += ix;
          error -= delta_y;
        }
        // else do nothing
      }
      // else do nothing

      y1 += iy;
      error += delta_x;

      plot(x1, y1);
    }
  }

}

// ######################################################################
void PointCloud2OccupancyModule::onMessage(PointCloudInput cloud)
{
  std::lock_guard<std::mutex> _(itsMtx);

  float const lowerThresh   = itsLowerThreshParam.getVal();
  float const upperThresh   = itsUpperThreshParam.getVal();
  float const ppm           = itsPixelsPerMeterParam.getVal();
  Dims<float> const mapDims = itsMapDimsParam.getVal();

  Image<PixGray<byte>, UniqueAccess> grid(mapDims.width()*ppm, mapDims.height()*ppm, ImageInitPolicy::Zeros);

  OccupancyVisitor visitor(grid);
  visitor.lowerThresh = lowerThresh;
  visitor.upperThresh = upperThresh;
  visitor.ppm         = ppm;
  
  cloud->cloud.applyInplaceVisitor(visitor);

  //int val = itsShadowsParam.getVal();
  //if(val < 0) val = 0;
  //if(val > 255) val = 255;
  //if(val > 0)
  int val = 255;
  {
    for(int i=0; i<360; i++)
      rasterLine(grid, grid.width()/2.0 + cos(i*M_PI/180.0)*grid.width(), grid.height()/2.0 + sin(i*M_PI/180.0)*grid.height(), val);
  }

  GenericImage genericGrid(Image<PixGray<byte>>(grid));

  // Post the pixels per meter message
  Message<nrt::real>::unique_ptr ppmMsg(new Message<nrt::real>(ppm));
  post<PixelsPerMeter>(ppmMsg);

  // Post the occupancy grid message
  std::unique_ptr<GenericImageMessage> gridMsg(
      new GenericImageMessage(GenericImage(Image<PixGray<byte>>(grid))));
  post<OccupancyGridOutput>(gridMsg);
}

NRT_REGISTER_MODULE(PointCloud2OccupancyModule);
