// UDPGameNetworking.cpp : Defines the entry point for the demo.
//

#include "UDPGameNetworking.h"
#include <iostream>
#include "Demo/Demo.h"

//Big TODO
// - Threading for server polling
int main()
{


	if (!NET_Init()) {
		return 0;
	}
	Demo* demo = new Demo();
	demo->Start();
	while (!demo->Done()) {
		demo->Update();
	}
	demo->Close();
	NET_Quit();
	return 0;
}

