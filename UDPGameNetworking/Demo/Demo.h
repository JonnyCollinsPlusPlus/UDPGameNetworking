#pragma once
#include "DemoWrapper.h"
#include "../Wrapper/IEngineObject.h"
class DemoPlayer : public IEngineObject {
private:
protected:
	DemoWrapper* wrapper;
	int x;
	int y;
public:
	DemoPlayer(DemoWrapper* wrapper);
	~DemoPlayer();
	void HandleInput(SDL_Event& e);
	void Update(float deltaTime);

	SDL_FRect GetRect();

	virtual void UpdateLibraryValues(std::vector<INetworkedValue*>* values) override;
	virtual void UpdateEngineValues(std::vector<INetworkedValue*>* values, LibSettings* settings) override;
};
class DemoClient {
private:
	bool started;
	bool isServer;
	DemoPlayer* clientPlayer;
	DemoColourSquare* colourSquare;
	DemoWrapper* wrapper;
	SDL_Window* window;
	SDL_Renderer* renderer;
protected:
public:
	DemoClient(bool isServer, int port, int lerpDelay, bool lerpEnabled);
	~DemoClient();
	void Start();
	void Update();
	void Close();

	void HandleInput(SDL_Event& e);
};
class Demo {
private:
	DemoClient* client1;
	DemoClient* client2;
	SDL_Window* guiWindow;
	SDL_Renderer* guiRenderer;
protected:
public:
	Demo();
	~Demo();
	void Start();
	void Update();
	void Close();

	bool Done();
};