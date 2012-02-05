#ifndef MYPOSTER_H
#define MYPOSTER_H

#include <nrt/Core/Blackboard/Module.H>

NRT_DECLARE_MESSAGEPOSTER_PORT(ChatterPort, nrt::Message<std::string>, void, "An important bit of chatter");
NRT_DECLARE_MESSAGEPOSTER_PORT(NumberPort,  nrt::Message<float>,       void, "A useful number");

class myPoster : public nrt::Module,
                 public nrt::MessagePoster<ChatterPort, NumberPort>
{
  public:
    myPoster(std::string const& instanceName="");

    virtual void run();
};

#endif // MYPOSTER_H

