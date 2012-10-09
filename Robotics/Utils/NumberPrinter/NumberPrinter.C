#include "NumberPrinter.H"

using namespace nrt;
using namespace numberprinter;

// ######################################################################
NumberPrinterModule::NumberPrinterModule(std::string const & instanceName) :
  Module(instanceName),
  itsLabelParam(LabelParam, this)
{ 
  //
}

// ######################################################################
void NumberPrinterModule::onMessage(numberprinter::InputMessage msg)
{
  std::lock_guard<std::mutex> _(itsMtx);
  NRT_INFO(itsLabelParam.getVal() << ": " << msg->value());
}

NRT_REGISTER_MODULE(NumberPrinterModule);
