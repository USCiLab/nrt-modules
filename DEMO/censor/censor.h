#ifndef CENSOR_H
#define CENSOR_H

#include <nrt/Core/Blackboard/Module.H>

NRT_DECLARE_MESSAGESUBSCRIBER_PORT(IncomingTextPort,   nrt::Message<std::string>, void, "Text to censor");
NRT_DECLARE_MESSAGEPOSTER_PORT(OutgoingTextPort, nrt::Message<std::string>, void, "Censored text");

class censor :	public nrt::Module,
								public nrt::MessageSubscriber<IncomingTextPort>,
                public nrt::MessagePoster<OutgoingTextPort>
								
{
  public:
    censor(std::string const& instanceName="");

    virtual void run();

		virtual void onMessage(IncomingTextPort msg);
		
	private:
		
		int readyToSend;
		std::string messageContents;
};

#endif // CENSOR_H

