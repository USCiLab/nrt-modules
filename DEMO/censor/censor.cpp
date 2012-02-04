#include "censor.h"
#include <iostream>

censor::censor(std::string const& instanceName) :
  nrt::Module(instanceName)
{
	readyToSend=0;
	messageContents = "";
}

void censor::onMessage(IncomingTextPort msg)
{
	readyToSend = 0;
  messageContents = msg->value;
  //NRT_INFO("I got some text: [" << messageContents << "]");
	std::string vulgars[] = {"damn","hate","hell","sucks"};

	for(int i=0; i<4; i++) {
		int found = messageContents.find(vulgars[i],0);
		if(found > 0) {
			NRT_INFO("found vulgarity");
			switch(i) {
				case 0:
					messageContents.erase(found,4);
					messageContents.insert(found,"darn");
					break;
				case 1:
					messageContents.erase(found,4);
					messageContents.insert(found,"love");
					break;
				case 2:
					messageContents.erase(found,4);
					messageContents.insert(found,"heck");
					break;
				case 3:
					messageContents.erase(found,5);
					messageContents.insert(found,"is too bad");
					break;
			}
		}
	}
	
	//NRT_INFO("Censored text: [" << messageContents << "]");
	readyToSend = 1;
}

void censor::run()
{
	while(running())
	{
		if(readyToSend > 0) {
			std::unique_ptr<nrt::Message<std::string>> stringMessagePtr(new nrt::Message<std::string>);
			stringMessagePtr->value = messageContents;

			post<OutgoingTextPort>(stringMessagePtr);
			readyToSend=0;
		}
	}
}

NRT_REGISTER_MODULE(censor);