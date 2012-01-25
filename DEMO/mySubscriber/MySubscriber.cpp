#include "MySubscriber.h"
#include <iostream>

MySubscriber::MySubscriber(std::string const& instanceName) :
  nrt::Module(instanceName)
{ }

void MySubscriber::onMessage(SomeTextPort msg)
{
  std::string messageContents = msg->value;
  NRT_INFO("I got some text: [" << messageContents << "]");
}

void MySubscriber::onMessage(SomeNumberPort msg)
{
  float messageContents = msg->value;
  NRT_INFO("I got some number: [" << messageContents << "]");
}

NRT_REGISTER_MODULE(MySubscriber);
