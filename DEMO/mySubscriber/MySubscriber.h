#ifndef MYSUBSCRIBER_H
#define MYSUBSCRIBER_H

#include <nrt/Core/Blackboard/Module.H>

NRT_DECLARE_MESSAGESUBSCRIBER_PORT(SomeTextPort,   nrt::Message<std::string>, void, "Some text to print");
NRT_DECLARE_MESSAGESUBSCRIBER_PORT(SomeNumberPort, nrt::Message<float>,       void, "Some number to print");

class MySubscriber : public nrt::Module,
                     public nrt::MessageSubscriber<SomeTextPort, SomeNumberPort>
{
  public:
    MySubscriber(std::string const& instanceName="");

    virtual void onMessage(SomeTextPort msg);

    virtual void onMessage(SomeNumberPort msg);
};

#endif // MYSUBSCRIBER_H

