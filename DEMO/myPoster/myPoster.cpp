#include "myPoster.h"
#include <iostream>
#include <string>

myPoster::myPoster(std::string const& instanceName) :
  nrt::Module(instanceName),
	itsRepeatsParam(RepeatsParamDef, this)
{ }

void myPoster::run()
{
  while(running())
  {
    // Send a string message
    std::string stringMessageContents;
    std::cout << "What string shall we send? ";
    std::cin >> stringMessageContents;

		for(int i = 0; i < itsRepeatsParam.getVal(); ++i)
		{
			std::unique_ptr<nrt::Message<std::string>> stringMessagePtr(new nrt::Message<std::string>);
			stringMessagePtr->value = stringMessageContents;

			post<ChatterPort>(stringMessagePtr);
		}


    // Send a number message
    float numberMessageContents;
    std::cout << "What number shall we send? ";
    std::cin >> numberMessageContents;

		for(int i = 0; i < itsRepeatsParam.getVal(); ++i)
		{
			std::unique_ptr<nrt::Message<float>> numberMessagePtr(new nrt::Message<float>);
			numberMessagePtr->value = numberMessageContents;

			post<NumberPort>(numberMessagePtr);
		}
  }
}

NRT_REGISTER_MODULE(myPoster);
