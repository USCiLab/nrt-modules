#include "KalmanFilterModule.H"

using namespace nrt;
using namespace kalmanfilter; 

// ######################################################################
KalmanFilterModule::KalmanFilterModule(std::string const& instanceName) :
  Module(instanceName),
  itsBParam(BParamDef, this),
  itsHParam(HParamDef, this),
  itsQParam(QParamDef, this),
  itsRParam(RParamDef, this)
{ }


// ######################################################################
void KalmanFilterModule::onMessage(RateMeasurement msg)
{
  std::lock_guard<std::mutex> _(itsMtx);

  itsX = itsX + itsBParam.getVal() * msg->value;
  itsP = itsP + itsQParam.getVal();

  std::unique_ptr<nrt::Message<nrt::real>> output(new nrt::Message<nrt::real>(itsX));
  post<FilteredOutput>(output);
}

// ######################################################################
void KalmanFilterModule::onMessage(AbsoluteMeasurement msg)
{
  std::lock_guard<std::mutex> _(itsMtx);

  nrt::real const H = itsHParam.getVal();
  nrt::real const R = itsRParam.getVal();

  nrt::real y = msg->value - H * itsX;
  nrt::real S = H*H*itsP + R;
  nrt::real K = itsP * H / S;
  itsX = itsX + K * y;
  itsP = (1 - K*H)*itsP;

  std::unique_ptr<nrt::Message<nrt::real>> output(new nrt::Message<nrt::real>(itsX));
  post<FilteredOutput>(output);
}

NRT_REGISTER_MODULE(KalmanFilterModule);

