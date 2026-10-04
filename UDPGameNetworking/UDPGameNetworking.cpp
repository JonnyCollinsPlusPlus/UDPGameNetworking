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
	Uint64 last = SDL_GetTicksNS();
	while (!demo->Done()) {
		Uint64 now = SDL_GetTicksNS();
		float deltaTime = (now-last) / 1'000'000'000.0f;
		last = now;
		demo->Update(deltaTime);
	}
	demo->Close();
	NET_Quit();
	return 0;
}

