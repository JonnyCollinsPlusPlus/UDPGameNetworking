#include "DemoGui.h"


void DemoGUI::Initialize()
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
}

void DemoGUI::Update()
{
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
}

void DemoGUI::Close()
{
    ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

void DemoGUI::HandleEvent(SDL_Event& e){
	ImGui_ImplSDL3_ProcessEvent(&e);

}