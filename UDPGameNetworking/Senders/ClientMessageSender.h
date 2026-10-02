#pragma once
#include "MessageSender.h"
class ClientMessageSender : public MessageSender {
private:
	int serverPort;
	NET_Address* server;
public:
	ClientMessageSender(NET_DatagramSocket* socket, EndpointInfo* serverInfo);
	void SendImportantMessage(NetworkMessageTypes type, std::string message);
};