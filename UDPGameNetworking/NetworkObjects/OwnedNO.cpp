#include "OwnedNO.h"
#include "../Wrapper/IWrapper.h"
#include "../Demo/ColourValue.h"
void OwnedNetworkObject::StreamSend(EndpointInfo* server, NET_DatagramSocket* socket, int clientTime, LibSettings* settings)
{
	engineObject->UpdateLibraryValues(networkedValues);
	std::string messageData = NetworkUtilities::AsBinaryString(ID, objectIDDigits);
	messageData.append(ObjectDataProcessor::ConstructDataStream(networkedValues, clientTime, settings));
	//std::cout << "message data: " << messageData << std::endl;
	NetworkUtilities::SendMessageTo(NetworkedObjectMsg, messageData, socket, server->address, server->port);
}

void OwnedNetworkObject::SendIDRequest(EndpointInfo* server, NET_DatagramSocket* socket)
{
	NetworkUtilities::SendMessageTo(IDRequest, "", socket, server->address, server->port);
}

void OwnedNetworkObject::SendInitializationMessage(EndpointInfo* server, NET_DatagramSocket* socket, MessageSender* sender, IWrapper* wrapper)
{
	//object id then
	// 8 bits object type
	// 8 bits value count
	//8 bits value id
	//8 bits value type
	//and so on
	std::string initInfo = "";
	//object ID
	initInfo += NetworkUtilities::AsBinaryString(ID, objectIDDigits);
	//object Type
	initInfo += NetworkUtilities::AsBinaryString(wrapper->EngineObjectMetadata(engineObject), 2);
	//value count
	initInfo += NetworkUtilities::AsBinaryString(networkedValues->size(), 2);
	for (int i = 0; i < networkedValues->size(); i++) {
		initInfo += wrapper->NetworkedValueMetadata(networkedValues->at(i));
	}
	NetworkUtilities::SendMessageTo(NetworkedObjectInit, initInfo, socket, server->address, server->port, sender);
}

OwnedNetworkObject::OwnedNetworkObject(EndpointInfo* server, NET_DatagramSocket* socket, IEngineObject* engineObj, IWrapper* wrapper)
{
	//Until the server confirms the ID, the ID will be 0 and object will remain uninitialized
	initialized = false;
	ID = 0;
	streamTimer = 0;
	SendIDRequest(server, socket);
	engineObject = engineObj;
	networkedValues = wrapper->ObjectInitialValues(engineObject);
}

OwnedNetworkObject::~OwnedNetworkObject()
{
	//Here we will attempt to send a message to the server notifying it of this object's deletion,
	// in the case that this message's sending is unsuccessful, the object will no longer be around
	// to resend it so the server side instance of the object will just be timed out

	//TODO Send a message to the server notifying of deletion
}

bool OwnedNetworkObject::IDRequestReceived(int newID, NET_DatagramSocket* socket, EndpointInfo* server, MessageSender* sender, IWrapper* wrapper)
{
	if (initialized || ID != 0) {
		return false;
	}
	ID = newID;
	std::cout << "Owned networked object initialization complete, ID assigned: " << ID << std::endl;
	initialized = true;
	SendInitializationMessage(server, socket, sender, wrapper);
	return true;
}

void OwnedNetworkObject::Update(float deltaTime, EndpointInfo* server, NET_DatagramSocket* socket, int clientTime, LibSettings* settings)
{
	if (!initialized) {
		//sometimes messages are lost and server doesnt send an ID confirmation, if so,
		// just ask again
		SendIDRequest(server, socket);
		return;
	}
	streamTimer += deltaTime;
	if (streamTimer > objectStreamRate) {
		streamTimer -= objectStreamRate;
		StreamSend(server, socket, clientTime, settings);
	}
}

