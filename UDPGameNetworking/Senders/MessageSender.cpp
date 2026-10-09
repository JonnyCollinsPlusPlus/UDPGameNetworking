#include "MessageSender.h"
#include "../Endpoints/Client.h"
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

MessageSender::MessageSender(NET_DatagramSocket* pSocket, LibSettings* settings, Client* ownedBy)
{
	owner = ownedBy;
	socket = pSocket;
	nextMessageID = 1;
	timeUntilResend = 0;
	resendMessageRate = settings->ackResendDelay;
}

void MessageSender::SendImportantMessageTo(std::string message, NetworkMessageTypes type, NET_Address* address, int port)
{
	SetMessageStatus("SENDING: initial send");
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

bool MessageSender::ShouldResendMessages(int deltaTime)
{
	timeUntilResend -= deltaTime;
	if (timeUntilResend < 0) {
		timeUntilResend += resendMessageRate;
		return true;
	}
	return false;
}

void MessageSender::SetMessageStatus(std::string status)
{
	if (!owner) {return;}
	owner->messageStatus = status;
}

void MessageSender::SendUnsentMessages(int deltaTime, bool skipCheck = false)
{
	if (!skipCheck) {
		if (!ShouldResendMessages(deltaTime)) {
			return;
		}
	}
	for (UnsentMessage* message : messages) {
		message->retries += 1;
		NetworkUtilities::SendMessageDirect(message->type, message->message, socket, message->target->address, message->target->port);
	}
}

void MessageSender::ConfirmationRecieved(NetworkMessage* confirmationMessage)
{
	int messageID = NetworkUtilities::IntFromBinaryString(confirmationMessage->GetExtraData().substr(0, 12), 3);
	auto message = find_if(messages.begin(), messages.end(), [messageID](UnsentMessage* m) {return (m->ID == messageID);});
	if (message == messages.end()){return;}
	std::string status = "SENDING: confirmation received after ";
	status.append(std::to_string((*message)->retries));
	status.append(" attempts");
	SetMessageStatus(status);
	delete *message;
	messages.erase(message);
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
