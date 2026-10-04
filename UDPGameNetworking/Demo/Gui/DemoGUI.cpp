#include "DemoGui.h"
#include "../Demo.h"

//returns true if anything in this section changed
static bool ClientLerpSection(const char* title, bool& lerpEnabled, int& delayMs)
{
	bool changed = false;

	ImGui::PushID(title);
	ImGui::SeparatorText(title);

	changed |= ImGui::Checkbox("Enable lerp", &lerpEnabled);

	ImGui::BeginDisabled(!lerpEnabled);
	ImGui::TextUnformatted("Delay (ms)");
	ImGui::SetNextItemWidth(-FLT_MIN);  
	changed |= ImGui::SliderInt("##delay", &delayMs, 0, 500, "%d ms");
	ImGui::EndDisabled();

	ImGui::PopID();
	return changed;
}

void DemoGUI::Initialize(Demo* demo)
{
	owner = demo;
	guiWindow = SDL_CreateWindow("UDP Networking Settings", 500, 500, SDL_WINDOW_RESIZABLE);
	guiRenderer = SDL_CreateRenderer(guiWindow, NULL);
	SDL_SetWindowPosition(guiWindow, 100, 100);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding    = ImVec2(18, 16);
	style.FramePadding     = ImVec2(10, 6);
	style.ItemSpacing      = ImVec2(10, 10);
	style.WindowBorderSize = 0.0f;
	style.WindowRounding   = 0.0f;
	style.FrameRounding    = 6.0f;
	style.GrabRounding     = 6.0f;
	style.GrabMinSize      = 16.0f;
	style.SeparatorTextBorderSize = 2.0f;
	style.SeparatorTextPadding    = ImVec2(10, 6);

	ImVec4* c = style.Colors;
	c[ImGuiCol_WindowBg]       = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
	c[ImGuiCol_FrameBg]        = ImVec4(0.20f, 0.20f, 0.24f, 1.00f);
	c[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.26f, 0.32f, 1.00f);
	c[ImGuiCol_FrameBgActive]  = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
	c[ImGuiCol_SliderGrab]       = ImVec4(0.35f, 0.62f, 0.95f, 1.00f);
	c[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.75f, 1.00f, 1.00f);
	c[ImGuiCol_CheckMark]        = ImVec4(0.35f, 0.62f, 0.95f, 1.00f);

	//font scaling stuff
	float scale = SDL_GetWindowDisplayScale(guiWindow);
	if (scale > 0.0f) {
		style.ScaleAllSizes(scale);
		style.FontScaleDpi = scale;
	}

	ImGui_ImplSDL3_InitForSDLRenderer(guiWindow, guiRenderer);
	ImGui_ImplSDLRenderer3_Init(guiRenderer);
}

void DemoGUI::Update()
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	const ImGuiViewport* vp = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(vp->WorkPos);
	ImGui::SetNextWindowSize(vp->WorkSize);

	const ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::Begin("Debug Panel", nullptr, flags);

	ImGui::TextColored(ImVec4(0.45f, 0.85f, 0.50f, 1.0f), "Network status: OK");
	ImGui::SameLine();
	ImGui::TextDisabled("(%.0f FPS)", ImGui::GetIO().Framerate);

	ImGui::SeparatorText("Network");
	ImGui::TextUnformatted("Simulated packet loss");
	ImGui::SetNextItemWidth(-FLT_MIN);
	if (ImGui::SliderInt("##packetloss", &packetLossValue, 0, 100, "%d %%"))
	{
		owner->SetPacketLoss(packetLossValue);
	}

	if (ClientLerpSection("Client 1", client1Lerp, client1LerpDelay))
	{
		owner->ApplyClient1Settings(client1Lerp, client1LerpDelay);
	}
	if (ClientLerpSection("Client 2", client2Lerp, client2LerpDelay))
	{
		owner->ApplyClient2Settings(client2Lerp, client2LerpDelay);
	}

	ImGui::End();

	ImGui::Render();
	SDL_SetRenderDrawColor(guiRenderer, 30, 30, 35, 255);
	SDL_RenderClear(guiRenderer);
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), guiRenderer);
	SDL_RenderPresent(guiRenderer);
}

void DemoGUI::Close()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(guiRenderer);
	SDL_DestroyWindow(guiWindow);
}

void DemoGUI::HandleEvent(SDL_Event& e)
{
	ImGui_ImplSDL3_ProcessEvent(&e);
}