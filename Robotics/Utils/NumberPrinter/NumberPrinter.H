#ifndef NUMBERPRINTER_H
#define NUMBERPRINTER_H

#include <nrt/Core/Blackboard/Module.H>

namespace numberprinter 
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(InputMessage, nrt::Message<nrt::real>, void, "The incoming message");
  
  static const nrt::ParameterCategory ParamCat("Message Display related options");

  static const nrt::ParameterDef<std::string> LabelParam("label", "Some descriptive text to prepend to the output", "", ParamCat);
}

class NumberPrinterModule: 
  public nrt::Module,
  public nrt::MessageSubscriber<numberprinter::InputMessage>
{
  public:
    NumberPrinterModule(std::string const& instanceName="");

    void onMessage(numberprinter::InputMessage msg);

  private:
    std::mutex itsMtx;
    nrt::Parameter<std::string> itsLabelParam;
};

#endif // NUMBERPRINTER_H 
