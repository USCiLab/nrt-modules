#include "myPoster.h"
#include <iostream>
#include <string>

myPoster::myPoster(std::string const& instanceName) :
  nrt::Module(instanceName)
{ }

void myPoster::run()
{
  while(running())
  {
    // Send a string message
    std::string stringMessageContents;
    std::cout << "What string shall we send? ";
    std::cin >> stringMessageContents;

		std::unique_ptr<nrt::Message<std::string>> stringMessagePtr(new nrt::Message<std::string>);
		stringMessagePtr->value = stringMessageContents;

		post<ChatterPort>(stringMessagePtr);
  }
}

NRT_REGISTER_MODULE(myPoster);
