#ifndef NUMBERPRINTER_H
#define NUMBERPRINTER_H

#include <nrt/Core/Blackboard/Module.H>

namespace numberposter 
{
  NRT_DECLARE_MESSAGEPOSTER_PORT(OutputMessage, nrt::Message<nrt::real>, void, "The port where numbers go out.");
  
  static const nrt::ParameterCategory ParamCat("Message Display related options");
  static const nrt::ParameterDef<std::string> NumbersParam("numbers", "A comma separated list of numbers to post.", "1,2,3,4,5", ParamCat);
  static const nrt::ParameterDef<double> RateParam("rate", "How fast to post numbers.", 1.0, ParamCat);
  static const nrt::ParameterDef<bool> RepeatParam("loop", "Whether or not to loop the numbers in the list.", true, ParamCat);
}

class NumberPosterModule: 
  public nrt::Module,
  public nrt::MessagePoster<numberposter::OutputMessage>
{
  public:
    NumberPosterModule(std::string const& instanceName="");

    void setNumbersCallback(std::string const & numbers);

    void run();

  private:
    std::mutex itsMtx;
    nrt::Parameter<std::string> itsNumberListParam;
    nrt::Parameter<double> itsRateParam;
    nrt::Parameter<bool> itsRepeatParam;
    std::vector<nrt::real> itsNumberList;
    std::vector<nrt::real>::iterator itsNumberListIterator;
    bool itShouldPost;
};

#endif // NUMBERPRINTER_H 
