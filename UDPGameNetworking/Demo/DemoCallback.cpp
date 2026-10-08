#include "DemoCallback.h"
#include <iostream>
DemoCallback::DemoCallback(int ID) : Callback(ID)
{
}
void DemoCallback::Invoke(std::string optionalExtraData) {
	std::cout << "Message Received: " << optionalExtraData << std::endl;
}