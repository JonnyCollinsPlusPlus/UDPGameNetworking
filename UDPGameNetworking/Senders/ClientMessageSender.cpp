#include "ClientMessageSender.h"


ClientMessageSender::ClientMessageSender(NET_DatagramSocket* socket, EndpointInfo* serverInfo, LibSettings* settings, Client* owner) : MessageSender(socket, settings, owner)
{
	server = serverInfo->address;
	serverPort = serverInfo->port;
}

void ClientMessageSender::SendImportantMessage(NetworkMessageTypes type, std::string message)
{
	MessageSender::SendImportantMessageTo(message, type, server, serverPort);
}
