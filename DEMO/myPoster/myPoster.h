#ifndef MYPOSTER_H
#define MYPOSTER_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Core/Model/Parameter.H>

NRT_DECLARE_MESSAGEPOSTER_PORT(ChatterPort, nrt::Message<std::string>, void, "An important bit of chatter");
NRT_DECLARE_MESSAGEPOSTER_PORT(NumberPort,  nrt::Message<float>,       void, "A useful number");

static const nrt::ParameterDef<int> RepeatsParamDef("repeats", "Number of times to repeat our post", 1,
                                                     {1, 2, 4, 8, 16});

class myPoster : public nrt::Module,
                 public nrt::MessagePoster<ChatterPort, NumberPort>
{
  public:
    myPoster(std::string const& instanceName="");

    virtual void run();

		nrt::Parameter<int> itsRepeatsParam;
};

#endif // MYPOSTER_H

