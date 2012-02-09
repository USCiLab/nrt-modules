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
		size_t found = messageContents.find(vulgars[i],0);
		if(found != std::string::npos) {
			NRT_INFO("found vulgarity");
			switch(i) {
				case 0:
					messageContents.erase(int(found),4);
					messageContents.insert(int(found),"darn");
					break;
				case 1:
					messageContents.erase(int(found),4);
					messageContents.insert(int(found),"love");
					break;
				case 2:
					messageContents.erase(int(found),4);
					messageContents.insert(int(found),"heck");
					break;
				case 3:
					messageContents.erase(int(found),5);
					messageContents.insert(int(found),"is too bad");
					break;
			}
		}
	}
	
	NRT_INFO("Censored text: [" << messageContents << "]");
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
