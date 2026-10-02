#pragma once
#include <string>
#include <queue>
#include "../Senders/ServerMessageSender.h"
#include "../Wrapper/Libsettings.h"
#include "../UDPGameNetworkingAPI.h"
class OwnedNetworkObject;
class UnownedNetworkObject;
#include "SocketHolder.h"
class UDPGAMENETWORKING_API Server : public SocketHolder{
private:
	NET_Address* address;
	int port;
	LibSettings* settings;

	ServerMessageSender* sender;
	std::vector<EndpointInfo*>* connectedClients;

	EndpointInfo* connectorInfo;
	bool connectingAClient;

	std::vector<OwnedNetworkObject*>* ownedObjects;
	std::vector<UnownedNetworkObject*>* nonOwnedObjects;
	std::queue<int>* availableIDs;

	int GetNextFreeID(); // for objects

	void ConfirmClientConnection(NetworkMessage* msg);
	void TryConnectClient(NetworkMessage* msg);
	bool IsAlreadyConnected(EndpointInfo* client);

	virtual void ProcessMessage(NetworkMessage* msg) override;

	void ProcessIncomingIDRequest(NetworkMessage* msg);
	void ProcessUserMessage(NetworkMessage* msg);
	void ProcessObjectMessage(NetworkMessage* msg);
	void InitializeNewObject(NetworkMessage* msg);
protected:
	virtual MessageSender* GetSender() override { return sender; }
public:
	Server(std::string ipAddress, int port, IWrapper* libWrapper, LibSettings* settings);
	void Update(float deltaTime);
	void Broadcast(NetworkMessageTypes type, std::string message);
	void ImportantBroadcast(NetworkMessageTypes type, std::string message);

	int GetConnectedClientCount();
	void SendMessageTo(NetworkMessageTypes type, std::string message, EndpointInfo* receiver);
	~Server();
};