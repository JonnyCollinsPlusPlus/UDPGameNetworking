#pragma once
#include "NetworkObject.h"
#include "../Endpoints/EndpointInfo.h"
#include "../NetworkUtilities.h"
#include "SDL3_net/SDL_net.h"
#include "../CustomStreaming/NetworkedValue.h"
#include "ObjectDataProcessor.h"
#include "../Wrapper/IEngineObject.h"
class IWrapper;
//Note that object IDs are sent in BCD format so ID digits is this / 4
static int objectIDBits = 8; // number of digits in an object ID
static int objectIDDigits = objectIDBits / 4;
static float objectStreamRate = 100; // one data stream message per X ms
static int streamedValueSize = 64; //number of bits per streamed value (8 for value ID, 56 for actual data)


//A network object owned by this client, it's incoming data is provided by the wrapper and it streams data
// to other clients
class OwnedNetworkObject : public NetworkObject {
private:
	//Owned network objects are created in an unitialized state, they request an ID from the server upon
	// creation and remain in this uninitialized state until the ID is provided by the server
	bool initialized;
	//tracks time since last stream message sent
	float streamTimer;
	//contains all values that will be streamed to the server, updated by updatevalues method in engine object
	std::vector<INetworkedValue*>* networkedValues;

	//used for managing data interfacing with the engine
	IEngineObject* engineObject;

	//Sends a data stream message
	void StreamSend(EndpointInfo* server, NET_DatagramSocket* socket, int clientTime, LibSettings* settings);
	//Sends the server a request for an ID
	void SendIDRequest(EndpointInfo* server, NET_DatagramSocket* socket);
	//sends initialization message detailing contained data types to server
	void SendInitializationMessage(EndpointInfo* server, NET_DatagramSocket* socket, MessageSender* sender, IWrapper* wrapper);
protected:
public:
	//Create the object in an uninitialized state and request an ID from the server
	OwnedNetworkObject(EndpointInfo* server, NET_DatagramSocket* socket, IEngineObject* engineObject, IWrapper* wrapper);
	~OwnedNetworkObject();
	//Initialize the object with an ID provided by the server
	//Returns true if object was initialized
	bool IDRequestReceived(int ID, NET_DatagramSocket* socket, EndpointInfo* server, MessageSender* sender, IWrapper* wrapper);

	//Updates the object, sending a message if required
	// @param deltaTime - the time in seconds since last update was called on this object
	void Update(float deltaTime, EndpointInfo* server, NET_DatagramSocket* socket, int clientTime, LibSettings* settings);

	int GetID() { return ID; }
};