#include "censor.h"
#include <iostream>

censor::censor(std::string const& instanceName) :
  nrt::Module(instanceName)
{ }

void censor::onMessage(IncomingTextPort msg)
{
  std::string messageContents = msg->value;
  NRT_INFO("I got some text: [" << messageContents << "]");
}

void censor::run()
{
	
}

NRT_REGISTER_MODULE(censor);
