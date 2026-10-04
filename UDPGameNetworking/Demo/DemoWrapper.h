#pragma once
#include "../Wrapper/IWrapper.h"
#include <SDL3/SDL.h>
#include <vector>
class ColourValue;
class Callback;
class DemoColourSquare;
class IEngineObject;
class DemoPlayer; //forward declaration
class DemoWrapper : public IWrapper {
private:
	int plannedPort;

	std::vector<DemoPlayer*>* otherPlayers;
	std::vector<DemoColourSquare*>* otherSquares;
	std::vector<Callback*>* registeredCallbacks;
protected:
public:
	DemoWrapper(int port, int lerpDelay, bool lerpEnabled);
	virtual void Update(float deltaTime) override;
	virtual void Initialize() override;
	virtual void RegisterObject(IEngineObject* obj) override;
	virtual void UnregisterObject(int ID) override;
	virtual void RegisterCallback(int ID) override;
	virtual void StartClient() override;
	virtual void StartServer() override;
	virtual void ApplySettings(LibSettings* s) override;
	virtual void InvokeRegisteredCallback(int callbackID, std::string optionalExtraData) override;
	virtual IEngineObject* NewNetworkedObject(int objectType, bool belongsToClient) override;
	virtual INetworkedValue* NewNetworkedValue(int valueID, int valueType) override;
	virtual std::string NetworkedValueMetadata(INetworkedValue* value) override;
	virtual int EngineObjectMetadata(IEngineObject* obj) override;
	virtual std::vector<INetworkedValue*>* ObjectInitialValues(IEngineObject* obj) override;
	void SetPacketLoss(int lossRate);



	void DrawOtherPlayers(SDL_Renderer* renderer);
	void CallbackTest();
	int GetClientTime();
};
