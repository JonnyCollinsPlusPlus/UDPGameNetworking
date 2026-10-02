#include "NetworkMessage.h"
#include "NetworkUtilities.h"
NetworkMessage::NetworkMessage()
{
}
NetworkMessage::NetworkMessage(NET_Datagram* datagramToProcess)
{
	Uint8* buffer = (Uint8*)datagramToProcess->buf;
	int bufferLength = datagramToProcess->buflen;
	std::string outData = NetworkUtilities::UnpackMessage(buffer, bufferLength);
	messageType = NetworkUtilities::UnpackHeader(outData);
	extraData = outData.substr(4);
	fromAddress = NET_RefAddress(datagramToProcess->addr);
	fromPort = datagramToProcess->port;
}

NetworkMessage::~NetworkMessage()
{
}

std::string NetworkMessage::Debug()
{
	return "The type of this message is: " + std::to_string(messageType) + " And the contents are: " + extraData;
}
std::string NetworkMessage::GetDataToForwardMessage()
{
	//for the base class this function is the same as getExtraData() as no trimming or changing is done to the extra data
	return GetExtraData();
}
NetworkMessageTypes NetworkMessage::GetMessageType() {
	return messageType;
}
std::string NetworkMessage::GetExtraData()
{
	return extraData;
}
NET_Address* NetworkMessage::GetAddress()
{
	return fromAddress;
}
int NetworkMessage::GetPort()
{
	return fromPort;
}

std::string NetworkMessage::RemoveStateVerification()
{
	if (extraData.size() < 4) {
		std::cout << "State verification remove from network message requested of a message smaller than 4 bits" << std::endl;
		return "";
	}
	std::string out = extraData.substr(0, 4);
	extraData = extraData.substr(4);
	return out;
}

EndpointInfo* NetworkMessage::GetSender()
{
	return new EndpointInfo(fromAddress, fromPort);
}
