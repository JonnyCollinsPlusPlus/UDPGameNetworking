#pragma once
#include "DemoWrapper.h"
#include "../Wrapper/IEngineObject.h"
#include "Gui/DemoGUI.h"
class DemoPlayer : public IEngineObject {
private:
protected:
	DemoWrapper* wrapper;
	int x;
	int y;
	int ghostX;
	int ghostY;
public:
	DemoPlayer(DemoWrapper* wrapper);
	~DemoPlayer();
	void HandleInput(SDL_Event& e);
	void Update(float deltaTime);

	SDL_FRect GetRect();
	SDL_FRect GetGhostRect();

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
	void Start(int clientNum);
	void Update(float deltaTime);
	void Close();

	void HandleInput(SDL_Event& e);
	void SetPacketLoss(int lossRate);
	void ApplySettings(LibSettings* s);
};
class Demo {
private:
	DemoClient* client1;
	DemoClient* client2;
	DemoGUI* gui;
protected:
public:
	Demo();
	~Demo();
	void Start();
	void Update(float deltaTime);
	void Close();

	bool Done();

	void SetPacketLoss(int lossRate);
	void ApplyClient1Settings(bool lerpEnabled, bool ghostEnabled, int lerpDelay, int ackResendDelay);
	void ApplyClient2Settings(bool lerpEnabled, bool ghostEnabled, int lerpDelay, int ackResendDelay);
};