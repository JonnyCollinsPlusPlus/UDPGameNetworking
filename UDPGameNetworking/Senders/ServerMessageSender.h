#pragma once
#include "MessageSender.h"
struct ImportantBroadcast : public UnsentMessage {
	std::vector<EndpointInfo*> unconfirmedClients;
	void ConfirmationReceived(NetworkMessage* confirmationMessage);
	bool IsFullyConfirmed();
	ImportantBroadcast(NetworkMessageTypes type, std::string message, std::vector<EndpointInfo*> clients, int ID);
};
struct ClientMessageChecker {
	EndpointInfo* client;
	std::vector<int> receivedMessages;
	//ClientInfo is deleted automatically
	ClientMessageChecker(EndpointInfo* c) {
		client = c;
	}
	~ClientMessageChecker() {
		delete client;
	}
};
class ServerMessageSender : public MessageSender {
private:
	std::vector<EndpointInfo*> clients;
	std::vector<ImportantBroadcast*> broadcasts;
	std::vector<ClientMessageChecker*> messageCheckers;
	void SendUnsentBroadcasts();
public:
	ServerMessageSender(NET_DatagramSocket* socket, std::vector<EndpointInfo*>* clients);
	void SendImportantMessage(NetworkMessageTypes type, std::string message, EndpointInfo* client);
	void BroadcastImportantMessage(NetworkMessageTypes type, std::string message);

	virtual void SendUnsentMessages(bool skipCheck) override;
	virtual void ConfirmationRecieved(NetworkMessage* message) override;

	void NewClientConnected(EndpointInfo* client);
	void ClientDisconnected(EndpointInfo* client);

	virtual ImportantMessage* ProcessImportantMessage(NetworkMessage* importantMessage) override;
};