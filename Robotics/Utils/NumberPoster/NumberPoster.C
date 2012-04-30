#include "NumberPoster.H"

using namespace nrt;
using namespace numberposter;

// ######################################################################
NumberPosterModule::NumberPosterModule(std::string const & instanceName) :
  Module(instanceName),
  itsNumberListParam(NumbersParam, this, &NumberPosterModule::setNumbersCallback),
  itsRateParam(RateParam, this),
  itsRepeatParam(RepeatParam, this)
{ 
  //
}

// ######################################################################
void NumberPosterModule::setNumbersCallback(std::string const & numbers)
{
  if (!initialized())
    return;

  std::lock_guard<std::mutex> _(itsMtx);
  std::vector<std::string> split = nrt::splitString(numbers, ',');
 
  itsNumberList.erase(itsNumberList.begin(), itsNumberList.end());
  for (std::string str : split)
    itsNumberList.push_back(boost::lexical_cast<nrt::real>(str));

  itsNumberListIterator = itsNumberList.begin();
  itShouldPost = true;
}

// ######################################################################
void NumberPosterModule::run()
{
  nrt::Timer timer;
  itShouldPost = true;

  while (running())
  {
    timer.start();
    int const ratetime_us = 1000000.0 / itsRateParam.getVal();

    if (itsRateParam.getVal() == 0)
    {
      sleep(1);
      continue;
    }

    {
      std::lock_guard<std::mutex> _(itsMtx);

      if (itShouldPost)
      {
        std::unique_ptr<nrt::Message<nrt::real>> msg(new nrt::Message<nrt::real>(*itsNumberListIterator));
        post<numberposter::OutputMessage>(msg);
        itsNumberListIterator++;
      }

      if (itsNumberListIterator == itsNumberList.end())
      {
        if (itsRepeatParam.getVal())
          itsNumberListIterator = itsNumberList.begin();
        itShouldPost = itsRepeatParam.getVal();
      }
    }

    int const time_us = timer.getDuration() * 1000000.0;
    if(ratetime_us - time_us > 0)
      usleep(ratetime_us - time_us);
  }
}

NRT_REGISTER_MODULE(NumberPosterModule);
