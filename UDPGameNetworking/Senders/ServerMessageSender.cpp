#include "ServerMessageSender.h"
void ServerMessageSender::BroadcastImportantMessage(NetworkMessageTypes type, std::string message) {
	if (clients.size() == 0) {
		std::cout << "Tried to send an important message but no clients were available to receive it! " << std::endl;
		return;
	}
	broadcasts.push_back(new ImportantBroadcast(type, message, clients, nextMessageID));
	IncrementNextMessage();
}
void ServerMessageSender::SendImportantMessage(NetworkMessageTypes type, std::string message, EndpointInfo* client)
{
	MessageSender::SendImportantMessageTo(message, type, client);
}
ServerMessageSender::ServerMessageSender(NET_DatagramSocket* socket, std::vector<EndpointInfo*>* connectedClients) : MessageSender(socket)
{
	clients = std::vector<EndpointInfo*>(*connectedClients);
}
void ServerMessageSender::ConfirmationRecieved(NetworkMessage* message)
{
	int messageID = NetworkUtilities::IntFromBinaryString(message->GetExtraData(), 3);
	messages.erase(remove_if(messages.begin(), messages.end(), [messageID](UnsentMessage* m) { if (m->ID == messageID) { delete m; return true; }; return false; }), messages.end());
	auto broadcast = find_if(broadcasts.begin(), broadcasts.end(), [messageID](ImportantBroadcast* b) {return b->ID == messageID; });
	if (broadcast != broadcasts.end()) {
		(*broadcast)->ConfirmationReceived(message);
		if ((*broadcast)->IsFullyConfirmed()) {
			ImportantBroadcast* broadcastToDelete = *broadcast;
			broadcasts.erase(remove(broadcasts.begin(), broadcasts.end(), *broadcast), broadcasts.end());
			delete broadcastToDelete;
		}
	}
	//TODO redo this so that commonalities are reused but the broadcasts are also processed
	//Is this really necessary if it works and shouldnt need to be edited ever again?
}

void ServerMessageSender::NewClientConnected(EndpointInfo* client)
{
	clients.push_back(new EndpointInfo(*client));
}

void ServerMessageSender::ClientDisconnected(EndpointInfo* client)
{
	messageCheckers.erase(remove_if(messageCheckers.begin(), messageCheckers.end(),
		[client](ClientMessageChecker* disconnector) {return (NET_GetAddressString(disconnector->client->address) == NET_GetAddressString(client->address) && disconnector->client->port == client->port); }),
		messageCheckers.end());
	clients.erase(remove_if(clients.begin(), clients.end(),
		[client](EndpointInfo* disconnector) {return (NET_GetAddressString(disconnector->address) == NET_GetAddressString(client->address) && disconnector->port == client->port); }),
		clients.end());
}

ImportantMessage* ServerMessageSender::ProcessImportantMessage(NetworkMessage* importantMessage)
{
	//send the confirmation regardless of wether or not the message is new
	NET_Address* address = importantMessage->GetAddress();
	int port = importantMessage->GetPort();
	NetworkUtilities::SendMessageTo(ImportantMessageConfirmation, importantMessage->GetDataToForwardMessage(), socket, address, port);
	//find the relevant message checker

	for (ClientMessageChecker* c : messageCheckers) {
		if (NET_GetAddressString(c->client->address) == NET_GetAddressString(importantMessage->GetAddress()) && c->client->port == importantMessage->GetPort()) {
			//check if the message checker contains the message and if so return the new message
			//construct an important message to read the message id
			ImportantMessage* message = new ImportantMessage(importantMessage);
			int messageID = message->GetMessageID();
			if (find(c->receivedMessages.begin(), c->receivedMessages.end(), messageID) == c->receivedMessages.end()) {
				c->receivedMessages.push_back(messageID);
				return message;
			}
			return nullptr;
		}
	}
	//new client sender, add new message checker
	messageCheckers.push_back(new ClientMessageChecker(new EndpointInfo(importantMessage->GetAddress(), importantMessage->GetPort())));
	return new ImportantMessage(importantMessage);
}

void ServerMessageSender::SendUnsentMessages(bool skipCheck = false)
{
	if (!skipCheck) {
		if (!ShouldResendMessages()) {
			return;
		}
	}
	SendUnsentBroadcasts();
	MessageSender::SendUnsentMessages(true);
}
void ServerMessageSender::SendUnsentBroadcasts()
{
	for (ImportantBroadcast* b : broadcasts) {
		for (EndpointInfo* c : b->unconfirmedClients) {
			SendMessageDirect(b->type, b->message, socket, c->address, c->port);
		}
	}
}

void ImportantBroadcast::ConfirmationReceived(NetworkMessage* confirmationMessage)
{
	auto client = find_if(unconfirmedClients.begin(),
		unconfirmedClients.end(),
		[confirmationMessage](EndpointInfo* c) {return (c->address == confirmationMessage->GetAddress() && c->port == confirmationMessage->GetPort()); });
	if (client == unconfirmedClients.end()) {
		return;
	}
	unconfirmedClients.erase(remove(unconfirmedClients.begin(), unconfirmedClients.end(), *client), unconfirmedClients.end());
}

bool ImportantBroadcast::IsFullyConfirmed()
{
	return unconfirmedClients.empty();
}

ImportantBroadcast::ImportantBroadcast(NetworkMessageTypes type, std::string message, std::vector<EndpointInfo*> clients, int ID)
	: UnsentMessage(message, nullptr, ID, type)
{
	unconfirmedClients = clients;
}
