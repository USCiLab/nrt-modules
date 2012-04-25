#include "OdomCalibrate.H"

using namespace nrt;
using namespace odomcalibrate;

// ######################################################################
OdomCalibrate::OdomCalibrate(std::string const & instanceName) :
  Module(instanceName),
  itsStartParam(StartParam, this, &OdomCalibrate::startParamCallback)
{
}

// ######################################################################
void OdomCalibrate::onMessage(DepthImage img)
{
  // compute the distance by averaging the D component in the middle 20x20 square
  if (itsStartParam.getVal())
  {
    auto image = img->img.convertTo<PixRGBD<float>>();
    int w = image.width();
    int h = image.height();
    float d = 0;

    if (w < 20 || h < 20)
    {
      d = image.at(w/2, h/2).d();
    }
    else
    {
      for (int j = h/2 - 10; j < h/2 + 10; j++)
      {
        for (int i = w/2 - 10; i < w/2 + 10; i++)
        {
          d += image.at(i, j).d();
        }
      }
      d = d/400;
    }
    itsCurrentDistance = d;
    NRT_INFO("Distance: " << d);
  }
}

// ######################################################################
void OdomCalibrate::startParamCallback(bool const & start)
{
  if (!start)
  {
    // do the regression and output the parameters
    for (std::pair<float, float> mapping : itsVelocityMapping)
    {
      NRT_INFO(mapping.first << ", " << mapping.second);
    }
  }
}

// ######################################################################
void OdomCalibrate::onMessage(VelocityCommand msg)
{
  if (itsStartParam.getVal())
  {
    nrt::Time now = nrt::now();

    float velocity = (itsCurrentDistance - itsLastDistance)/std::chrono::duration_cast<std::chrono::seconds>(now - itsLastMeasurementTime).count();
    itsVelocityMapping.push_back(std::make_pair(msg->linear.x(), velocity));

    itsLastDistance = itsCurrentDistance;
    itsLastMeasurementTime = now;
  }
}

NRT_REGISTER_MODULE(OdomCalibrate);
