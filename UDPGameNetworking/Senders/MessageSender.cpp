#include "MessageSender.h"

void MessageSender::SendMessageDirect(NetworkMessageTypes type, std::string message, NET_DatagramSocket* socket, NET_Address* address, int port)
{
	NetworkUtilities::SendMessageDirect(type, message, socket, address, port);
}

void MessageSender::IncrementNextMessage()
{
	nextMessageID++;
	if (nextMessageID > 999) {
		nextMessageID = 0;
	}
}

MessageSender::MessageSender(NET_DatagramSocket* pSocket)
{
	socket = pSocket;
	nextMessageID = 1;
	updatesUntilResendMessages = 0;
	resendMessageRate = 100;
}

void MessageSender::SendImportantMessageTo(std::string message, NetworkMessageTypes type, NET_Address* address, int port)
{
	UnsentMessage* msg = new UnsentMessage(message, new EndpointInfo(address, port), nextMessageID, type);
	messages.push_back(msg);
	IncrementNextMessage();
}

void MessageSender::SendImportantMessageTo(std::string message, NetworkMessageTypes type, EndpointInfo* client)
{
	UnsentMessage* msg = new UnsentMessage(message, new EndpointInfo(*client), nextMessageID, type);
	messages.push_back(msg);
	IncrementNextMessage();

}

bool MessageSender::ShouldResendMessages()
{
	updatesUntilResendMessages--;
	if (updatesUntilResendMessages < 0) {
		updatesUntilResendMessages = resendMessageRate;
		return true;
	}
	return false;
}

void MessageSender::SendUnsentMessages(bool skipCheck = false)
{
	if (!skipCheck) {
		if (!ShouldResendMessages()) {
			return;
		}
	}
	for (UnsentMessage* message : messages) {

		NetworkUtilities::SendMessageDirect(message->type, message->message, socket, message->target->address, message->target->port);
	}
}

void MessageSender::ConfirmationRecieved(NetworkMessage* confirmationMessage)
{
	int messageID = NetworkUtilities::IntFromBinaryString(confirmationMessage->GetExtraData().substr(0, 12), 3);
	messages.erase(remove_if(messages.begin(), messages.end(), [messageID](UnsentMessage* m) {if (m->ID == messageID) { delete m; return true; } return false; }), messages.end());
}
ImportantMessage* MessageSender::ProcessImportantMessage(NetworkMessage* importantMessage)
{
	//send the confirmation regardless of wether or not the message is new
	NET_Address* address = importantMessage->GetAddress();
	int port = importantMessage->GetPort();
	NetworkUtilities::SendMessageTo(ImportantMessageConfirmation, importantMessage->GetExtraData(), socket, address, port);
	//check if the message is new, and if so, register it and return true
	ImportantMessage* message = new ImportantMessage(importantMessage);
	int messageID = message->GetMessageID();
	if (find(receivedMessages.begin(), receivedMessages.end(), messageID) == receivedMessages.end()) {
		receivedMessages.push_back(messageID);
		return message;
	}
	return nullptr;
}
