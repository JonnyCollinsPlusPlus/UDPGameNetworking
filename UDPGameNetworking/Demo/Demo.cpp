#include "Demo.h"
#include "DemoColourSquare.h"
#include "../CustomStreaming/PositionLerp2D.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
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
	guiWindow = SDL_CreateWindow("UDP Game Networking Demo client", 500, 500, 0);
	guiRenderer = SDL_CreateRenderer(guiWindow, NULL);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	// 2. Init Backends (after SDL Window & Renderer creation)
	ImGui_ImplSDL3_InitForSDLRenderer(guiWindow, guiRenderer);
	ImGui_ImplSDLRenderer3_Init(guiRenderer);

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
	client1->Start();
	client2->Start();
}

void Demo::Update()
{
	
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		ImGui_ImplSDL3_ProcessEvent(&e);

		client1->HandleInput(e);
		client2->HandleInput(e);
	}
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug Panel");
    ImGui::Text("Network status: OK");
    ImGui::End();

	ImGui::Render();
	SDL_SetRenderDrawColor(guiRenderer, 40, 40, 40, 255);
    SDL_RenderClear(guiRenderer);

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), guiRenderer);

	SDL_RenderPresent(guiRenderer);
	
	client1->Update();
	client2->Update();
}

void Demo::Close()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

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
