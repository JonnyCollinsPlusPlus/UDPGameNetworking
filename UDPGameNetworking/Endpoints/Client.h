#pragma once
#include "../Senders/ClientMessageSender.h"
#include "SocketHolder.h"
#include "../Wrapper/LibSettings.h"
#include "../UDPGameNetworkingAPI.h"
class IEngineObject;
class OwnedNetworkObject;
class UnownedNetworkObject;
class IWrapper;
class UDPGAMENETWORKING_API Client  : public SocketHolder{
private:
	EndpointInfo* serverInfo;
	int port;
	bool isConnected;
	bool connecting;
	int fallbackConnectionRequestDelay;
	LibSettings* settings;

	ClientMessageSender* sender;
	std::vector<OwnedNetworkObject*>* ownedObjects;
	std::vector<UnownedNetworkObject*>* nonOwnedObjects;

	virtual void ProcessMessage(NetworkMessage* msg) override;

	void ProcessIncomingIDRequest(NetworkMessage* msg);
	void ProcessUserMessage(NetworkMessage* msg);
	void ProcessCallback(NetworkMessage* msg);

	void ProcessObjectMessage(NetworkMessage* msg);
	void InitializeNewObject(NetworkMessage* msg);

	void UpdateObjects(float deltaTime);

	bool AmIThisObjectsOwner(int objectID);

	void SendConnectRequest();
protected:
	virtual MessageSender* GetSender() override { return sender; }
public:
	Client(int portToUse, IWrapper* libWrapper, LibSettings* settings);
	~Client();	
	
	void ConnectToServer(std::string serverAddress);
	//disconnects from a server (with no warning to the server) and cleares network pointers
	void Disconnect();
	//process incoming packets, stream owned object data, and trigger relevant callbacks
	void Update(float deltaTime);
	bool IsConnected();
	void SendServerMessage(NetworkMessageTypes type, std::string msg);

	void RegisterObject(IEngineObject* obj);

	void SetSimulatedPacketLoss(int lossRate);
	std::string messageStatus;
};