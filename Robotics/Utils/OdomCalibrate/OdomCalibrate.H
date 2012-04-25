#ifndef NRT_MODULES_ROBOTICS_UTILS_ODOMCALIBRATE_H
#define NRT_MODULES_ROBOTICS_UTILS_ODOMCALIBRATE_H

#include <nrt/Core/Blackboard/Module.H>
#include <Messages/ImageProc/GenericImageMessage.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>

namespace odomcalibrate
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(VelocityCommand, nrt::VelocityMessage, void, "Velocity commanded to the robot.");
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(DepthImage, GenericImageMessage, void, "RGBD data for distance measurement.");

  static const nrt::ParameterCategory ParamCat("OdomCalibrate related options");

  static const nrt::ParameterDef<bool> StartParam("start", "Set this to true to start the calibration", false, ParamCat);
}

class OdomCalibrate : public nrt::Module,
  public nrt::MessageSubscriber<odomcalibrate::VelocityCommand, odomcalibrate::DepthImage>
{
  public:
    OdomCalibrate(std::string const & instanceName="");

    void onMessage(odomcalibrate::DepthImage img);

    void onMessage(odomcalibrate::VelocityCommand msg);

  private:
    void startParamCallback(bool const & start);

    nrt::Parameter<bool> itsStartParam;
    std::vector<std::pair<float, float>> itsVelocityMapping;
    nrt::Time itsLastMeasurementTime;
    float itsCurrentDistance;
    float itsLastDistance;
};

#endif // NRT_MODULES_ROBOTICS_UTILS_ODOMCALIBRATE_H
