#include "Demo.h"
#include "DemoColourSquare.h"
#include "../CustomStreaming/PositionLerp2D.h"
DemoClient::DemoClient(bool server, int port, int lerpDelay, bool lerpEnabled)
{
	isServer = server;
	wrapper = new DemoWrapper(port, lerpDelay, lerpEnabled);
	clientPlayer = new DemoPlayer(wrapper);
	colourSquare = new DemoColourSquare(wrapper, 50);
	started = false;
	window = nullptr;
	renderer = nullptr;
}

DemoClient::~DemoClient()
{
	delete clientPlayer;
	delete colourSquare;
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
}

void DemoClient::Start()
{



	wrapper->Initialize();
	if (isServer) {
		wrapper->StartServer();
	}
	wrapper->StartClient();
	wrapper->RegisterObject(clientPlayer);
	wrapper->RegisterObject(colourSquare);
	wrapper->RegisterCallback(500);
	started = true;
	window = SDL_CreateWindow("UDP Game Networking Demo client", 500, 500, 0);
	renderer = SDL_CreateRenderer(window, NULL);
}

void DemoClient::Update()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 0);
	const SDL_FRect rect = clientPlayer->GetRect();
	SDL_RenderRect(renderer, &rect);
	wrapper->DrawOtherPlayers(renderer);
	colourSquare->Render(renderer);
	SDL_RenderPresent(renderer);
	wrapper->Update(1);
}
void DemoClient::Close()
{
}


void DemoClient::HandleInput(SDL_Event& e)
{
	if (SDL_GetKeyboardFocus() == window) {
		clientPlayer->HandleInput(e);
		colourSquare->HandleInput(e);
	}
}

Demo::Demo()
{

	//2 clients each with a different port, 100ms lerp delay, and lerping enabled
	client1 = new DemoClient(true, 55511, 100, true);
	client2 = new DemoClient(false, 55522, 100, true);


}

Demo::~Demo()
{
	delete client1;
	delete client2;
}

void Demo::Start()
{
	gui = new DemoGUI();
	gui->Initialize();
	client1->Start();
	client2->Start();
}

void Demo::Update()
{
	
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		gui->HandleEvent(e);
		client1->HandleInput(e);
		client2->HandleInput(e);
	}
	gui->Update();
	client1->Update();
	client2->Update();
}

void Demo::Close()
{
	gui->Close();
	client1->Close();
	client2->Close();
}

bool Demo::Done()
{
	return false;
}

DemoPlayer::DemoPlayer(DemoWrapper* libWrapper)
{
	x = 5;
	y = 5;
	wrapper = libWrapper;
}

DemoPlayer::~DemoPlayer()
{
}

void DemoPlayer::HandleInput(SDL_Event& e)
{
	switch (e.type) {
	case SDL_EVENT_KEY_DOWN:
		if (e.key.key == SDLK_A) {
			x -= 5;
		}
		if (e.key.key == SDLK_D) {
			x += 5;
		}
		if (e.key.key == SDLK_W) {
			y -= 5;
		}
		if (e.key.key == SDLK_S) {
			y += 5;
		}
		if (e.key.key == SDLK_M) {
			wrapper->CallbackTest();
		}
	}
}

void DemoPlayer::Update(float deltaTime)
{

}

SDL_FRect DemoPlayer::GetRect()
{
	return SDL_FRect{ (float)x, (float)y, 20, 20 };
}

void DemoPlayer::UpdateLibraryValues(std::vector<INetworkedValue*>* values)
{
	((PositionLerp2D*)values->at(0))->UpdateValue(x, y);
}

void DemoPlayer::UpdateEngineValues(std::vector<INetworkedValue*>* values, LibSettings* settings)
{
	Position* p = ((PositionLerp2D*)values->at(0))->GetCurrentValue(wrapper->GetClientTime(), settings);
	x = p->x;
	y = p->y;
	delete p;
}
