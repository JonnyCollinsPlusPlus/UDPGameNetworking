#pragma once
#include <string>
#include "SDL3_net/SDL_net.h"
#include "Endpoints/EndpointInfo.h"
enum NetworkMessageTypes;
class NetworkMessage {
private:
protected:
	NetworkMessageTypes messageType;
	NET_Address* fromAddress;
	int fromPort;
	std::string extraData;
public:
	NetworkMessage();
	NetworkMessage(NET_Datagram* datagramToProcess);
	virtual ~NetworkMessage();
	virtual std::string Debug();
	//Gets the extra data, but also includes data that may be trimmed off so that the message can be forwarded unwarped
	virtual std::string GetDataToForwardMessage();
	NetworkMessageTypes GetMessageType();
	std::string GetExtraData();
	NET_Address* GetAddress();
	int GetPort();
	std::string RemoveStateVerification();
	//IMPORTANT: returned value must be deleted
	EndpointInfo* GetSender();
};