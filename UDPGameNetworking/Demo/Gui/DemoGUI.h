#pragma once
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>
class Demo;
class DemoGUI{
private:
	SDL_Window* guiWindow;
	SDL_Renderer* guiRenderer;
    Demo* owner;

    int packetLossValue;
    bool client1Lerp;
    bool client1Ghost;
    int client1LerpDelay;
    bool client2Lerp;
    bool client2Ghost;
    int client2LerpDelay;
    int ackResendValue = 100;
protected:
public:
    void Initialize(Demo* demo);
    void Update();
    void Close();
    void HandleEvent(SDL_Event& e);
};