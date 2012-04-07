#ifndef NRT_MODULES_ROBOTICS_SENSING_FILTERS_KALMANFILTERMODULE_H
#define NRT_MODULES_ROBOTICS_SENSING_FILTERS_KALMANFILTERMODULE_H 

#include <nrt/Core/Blackboard/Module.H>

namespace kalmanfilter
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(RateMeasurement,     nrt::Message<nrt::real>, void, "A measurement of the current rate of the system" );
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(AbsoluteMeasurement, nrt::Message<nrt::real>, void, "A measurement of the absolute position of the system" );
  NRT_DECLARE_MESSAGEPOSTER_PORT(FilteredOutput,          nrt::Message<nrt::real>, void, "The Kalman filtered output" );

  static const nrt::ParameterCategory ParamCat("Kalman Filter Related Options");
  static const nrt::ParameterDef<nrt::real> BParamDef("B", "The conversion from RateMeasurement units to Output units", 1, ParamCat);
  static const nrt::ParameterDef<nrt::real> HParamDef("H", "The conversion from Output units to AbsoluteMeasurement units", 1, ParamCat);
  static const nrt::ParameterDef<nrt::real> QParamDef("Rate Sensor Noise", "The amount of variance in the RateMeasurements", 0.1, ParamCat);
  static const nrt::ParameterDef<nrt::real> RParamDef("Absolute Sensor Noise", "The amount of variance in the AbsoluteMeasurements", 0.1, ParamCat);
}

//! A simple 1 dimensional linear Kalman filter 
/*! The Kalman filter module is useful for filtering data from a rate sensor and an absolute sensor. 
   
    \author Randolph Voorhies
    \keywords kalman filter, filtering, sensing, bayes, bayesian */
class KalmanFilterModule : public nrt::Module,
                           public nrt::MessageSubscriber<kalmanfilter::RateMeasurement,
                                                         kalmanfilter::AbsoluteMeasurement>,
                           public nrt::MessagePoster<kalmanfilter::FilteredOutput>
{
  public:
    KalmanFilterModule(std::string const& instanceName="");

    void onMessage(kalmanfilter::RateMeasurement msg);
    void onMessage(kalmanfilter::AbsoluteMeasurement msg);

  private:
    std::mutex itsMtx;
    nrt::real itsX;
    nrt::real itsP;
    nrt::Parameter<nrt::real> itsBParam;
    nrt::Parameter<nrt::real> itsHParam;
    nrt::Parameter<nrt::real> itsQParam;
    nrt::Parameter<nrt::real> itsRParam;
    nrt::Time itsLastTime;
};

#endif // NRT_MODULES_ROBOTICS_SENSING_FILTERS_KALMANFILTERMODULE_H 
