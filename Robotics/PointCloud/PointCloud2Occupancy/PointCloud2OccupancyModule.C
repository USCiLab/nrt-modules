#include "PointCloud2OccupancyModule.H"

using namespace pointcloud2occupancy;
using namespace nrt;

// ######################################################################
PointCloud2OccupancyModule::PointCloud2OccupancyModule(std::string const & instanceId) :
  Module(instanceId),
  itsLowerThreshParam(lowerThresholdParamDef, this),
  itsUpperThreshParam(upperThresholdParamDef, this),
  itsPixelsPerMeterParam(pixelsPerMeterParamDef, this),
  itsMapDimsParam(mapDimsParamDef, this)
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

// ######################################################################
void PointCloud2OccupancyModule::onMessage(PointCloudInput cloud)
{
  std::lock_guard<std::mutex> _(itsMtx);

  float const lowerThresh   = itsLowerThreshParam.getVal();
  float const upperThresh   = itsUpperThreshParam.getVal();
  float const ppm           = itsPixelsPerMeterParam.getVal();
  Dims<float> const mapDims = itsMapDimsParam.getVal();

  Message<nrt::real>::unique_ptr ppmMsg(new Message<nrt::real>(ppm));
  post<PixelsPerMeter>(ppmMsg);

  Image<PixGray<byte>, UniqueAccess> grid(mapDims.width()*ppm, mapDims.height()*ppm, ImageInitPolicy::Zeros);

  OccupancyVisitor visitor(grid);
  visitor.lowerThresh = lowerThresh;
  visitor.upperThresh = upperThresh;
  visitor.ppm         = ppm;
  
  cloud->cloud.applyInplaceVisitor(visitor);
  GenericImage genericGrid(Image<PixGray<byte>>(grid));
  std::unique_ptr<GenericImageMessage> gridMsg(
      new GenericImageMessage(GenericImage(Image<PixGray<byte>>(grid))));
  post<OccupancyGridOutput>(gridMsg);
}

NRT_REGISTER_MODULE(PointCloud2OccupancyModule);
