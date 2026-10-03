#pragma once
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>
class DemoGUI{
private:
	SDL_Window* guiWindow;
	SDL_Renderer* guiRenderer;
protected:
public:
    void Initialize();
    void Update();
    void Close();
    void HandleEvent(SDL_Event& e);
};