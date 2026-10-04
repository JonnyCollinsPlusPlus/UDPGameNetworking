#include "Demo.h"
#include "DemoColourSquare.h"
#include "../CustomStreaming/PositionLerp2D.h"
#include <string>
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

void DemoClient::Start(int clientNum)
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
	std::string windowName = "Client " + std::to_string(clientNum);
	window = SDL_CreateWindow(windowName.c_str(), 500, 500, 0);
	renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetWindowPosition(window, (clientNum * 500) + 100, 100);

}

void DemoClient::Update(float deltaTime)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 0);
	const SDL_FRect rect = clientPlayer->GetRect();
	SDL_RenderFillRect(renderer, &rect);
	wrapper->DrawOtherPlayers(renderer);
	colourSquare->Render(renderer);
	SDL_RenderPresent(renderer);
	wrapper->Update(deltaTime);
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
void DemoClient::SetPacketLoss(int lossRate){
	wrapper->SetPacketLoss(lossRate);
}
void DemoClient::ApplySettings(LibSettings* s){
	wrapper->ApplySettings(s);
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
	gui->Initialize(this);
	client1->Start(1);
	client2->Start(2);
}

void Demo::Update(float deltaTime)
{
	
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		gui->HandleEvent(e);
		client1->HandleInput(e);
		client2->HandleInput(e);
	}
	gui->Update();
	//inside clients deltaTime is in ms
	client1->Update(deltaTime * 1000);
	client2->Update(deltaTime * 1000);
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

void Demo::SetPacketLoss(int lossRate){
	client1->SetPacketLoss(lossRate);
}

void Demo::ApplyClient1Settings(bool lerpEnabled, int lerpDelay){
	LibSettings* s = new LibSettings();
	s->lerpDelay = lerpDelay;
	s->lerpEnabled = lerpEnabled;
	client1->ApplySettings(s);
	delete s;
}
void Demo::ApplyClient2Settings(bool lerpEnabled, int lerpDelay){
	LibSettings* s = new LibSettings();
	s->lerpDelay = lerpDelay;
	s->lerpEnabled = lerpEnabled;
	client2->ApplySettings(s);
	delete s;
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
SDL_FRect DemoPlayer::GetGhostRect(){
	return SDL_FRect{ (float)ghostX, (float)ghostY, 20, 20};
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


	Position* gp =  ((PositionLerp2D*)values->at(0))->GetMostRecentValue();
	ghostX = gp->x;
	ghostY = gp->y;
	delete gp;
}

