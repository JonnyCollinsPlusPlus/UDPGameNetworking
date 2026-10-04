#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "LibSettings.h"
#include "../UDPGameNetworkingAPI.h"
class Client;
class Server;
class IEngineObject;
class INetworkedValue;
//This class should be inherited by wrapper to provide service specific
// functionality relating to registering values for consistent broadcasting
class UDPGAMENETWORKING_API IWrapper {
private:
protected:
	//objects being registered, currently waiting for an ID from the server
	std::vector<IEngineObject*> registeringOwnedObjects;
	//objects that have been registered, and are currently being streamed
	std::unordered_map<int, IEngineObject*> registeredOwnedObjects;
	//objects that other clients have registered, and are (hopefully) being rendered by this client
	std::unordered_map<int, IEngineObject*> registeredUnownedObjects;
	//Manages networking and server connection
	Client* client;
	//Not necessary for every client, manages multiple client connections
	Server* server;

	//This class manages and controls the settings
	LibSettings* settings;
public:
	// --- To be called by engine plugin/engine ---
	virtual void Update(float deltaTime) = 0;
	virtual void Initialize() = 0;
	virtual void RegisterObject(IEngineObject*) = 0;
	virtual void UnregisterObject(int ID) = 0;
	virtual void RegisterCallback(int ID) = 0;
	virtual void StartClient() = 0;
	virtual void StartServer() = 0;
	virtual void ApplySettings(LibSettings* s) = 0;
	// --- To be called by library ---
	virtual void InvokeRegisteredCallback(int callbackID, std::string optionalExtraData) = 0;
	//This should define how the engine should respond when another client, or the server,
	// sends information about a new object (typically should instantiate a new object of 
	// the specified type ready for streaming)
	//returns the engine object to continue data streaming with for this new object
	virtual IEngineObject* NewNetworkedObject(int objectType, bool belongsToClient) = 0;
	//This should define how engine object IDs and types are streamed
	virtual int EngineObjectMetadata(IEngineObject* obj) = 0;
	//This defines how values of an object are initialized
	//All streamed types must be constructed in here depending on IDs
	virtual INetworkedValue* NewNetworkedValue(int valueID, int typeID) = 0;
	//This defines how types are streamed initially, should match NewNetworkedValue method
	virtual std::string NetworkedValueMetadata(INetworkedValue* val) = 0;
	//This defines how objects values are initially constructed
	virtual std::vector<INetworkedValue*>* ObjectInitialValues(IEngineObject* obj) = 0;
};