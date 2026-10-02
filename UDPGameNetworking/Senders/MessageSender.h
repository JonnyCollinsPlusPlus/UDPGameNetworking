#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include "../NetworkUtilities.h"
#include "../Endpoints/EndpointInfo.h"
#include "../ImportantMessage.h"
struct UnsentMessage {
	int ID;
	EndpointInfo* target;
	NetworkMessageTypes type;
	std::string message;
	UnsentMessage(std::string msg, EndpointInfo* msgTarget, int id, NetworkMessageTypes t) {
		message = NetworkUtilities::AsBinaryString(id, 3) + msg;
		ID = id;
		target = msgTarget;
		type = t;
	}
	~UnsentMessage() {
		delete target;
	}
};
class MessageSender {
private:
	int updatesUntilResendMessages;
	int resendMessageRate;
protected:
	NET_DatagramSocket* socket;
	std::vector<UnsentMessage*> messages;
	std::vector<int> receivedMessages;
	void IncrementNextMessage();
	int nextMessageID;
	MessageSender(NET_DatagramSocket* socket);
	void SendImportantMessageTo(std::string message, NetworkMessageTypes type, EndpointInfo* client);
	void SendMessageDirect(NetworkMessageTypes type, std::string message, NET_DatagramSocket* socket, NET_Address* address, int port);
	bool ShouldResendMessages();
public:
	virtual void SendUnsentMessages(bool skipCheck);

	virtual void ConfirmationRecieved(NetworkMessage* confirmationMessage);
	//returns a pointer to the message as an important message and sends confirmation of the message being sent
	//Note that the confirmation message is sent regardless of wether or not it is the first time since the confirmation may not be received
	//Also note that the parameter pointer will be deleted
	//Implementation should look something like this:
	//message = sender->ProcessImportantMessage(message);
	//if (!message) {
	//	return;
	//}
	virtual ImportantMessage* ProcessImportantMessage(NetworkMessage* importantMessage);
	void SendImportantMessageTo(std::string message, NetworkMessageTypes type, NET_Address* address, int port);
};
