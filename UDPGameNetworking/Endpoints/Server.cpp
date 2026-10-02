#include "Server.h"
#include "../Wrapper/IWrapper.h"
#include "../NetworkObjects/OwnedNO.h"
#include "../NetworkObjects/UnownedNO.h"
#include "../Senders/ServerMessageSender.h"
int Server::GetNextFreeID()
{
    int out = availableIDs->front();
    availableIDs->pop();
    if (availableIDs->size() == 0) {
        availableIDs->push(out + 1);
    }
    return out;
}
void Server::ConfirmClientConnection(NetworkMessage* msg)
{
    EndpointInfo* client = msg->GetSender();
    if (!connectingAClient) {
        delete client;
        return;
    }
    if (*client != *connectorInfo) {
        delete client;
        return;
    }
    connectingAClient = false;
    connectedClients->push_back(client);
    sender->NewClientConnected(client);
    connectorInfo = nullptr;
    std::cout << "Successfully connected client from address: " << NET_GetAddressString(client->address) << std::endl;
}

void Server::TryConnectClient(NetworkMessage* msg)
{
    EndpointInfo* client = msg->GetSender();
    if (IsAlreadyConnected(client)) {
        delete client;
        return;
    }
    if (connectingAClient) {
        delete client;
        return;
    }
    connectingAClient = true;
    connectorInfo = client;
    NetworkUtilities::SendMessageTo(Connect, "", socket, connectorInfo->address, connectorInfo->port, sender);
}

bool Server::IsAlreadyConnected(EndpointInfo* client)
{
    for (EndpointInfo* connectedClient : *connectedClients) {
        if (client == connectedClient) {
            return true;
        }
    }
    return false;
}


void Server::ProcessMessage(NetworkMessage* msg)
{
    switch (msg->GetMessageType()) {
    case UserImportant:
        ProcessUserMessage(msg);
        return;
    case UserUnImportant:
        ProcessUserMessage(msg);
        return;
    case IDRequest:
        ProcessIncomingIDRequest(msg);
        return;
    case NetworkedObjectInit:
        InitializeNewObject(msg);
    case NetworkedObjectMsg:
        ProcessObjectMessage(msg);
        return;
    case Connect:
        TryConnectClient(msg);
        return;
    case ConnectConfirm:
        ConfirmClientConnection(msg);
        return;
    }
}

void Server::ProcessIncomingIDRequest(NetworkMessage* msg)
{
    std::string idValue = NetworkUtilities::AsBinaryString(GetNextFreeID(), objectIDDigits);
    NetworkUtilities::SendMessageTo(IDRequest, idValue, socket, msg->GetAddress(), msg->GetPort(), sender);
}

void Server::ProcessUserMessage(NetworkMessage* msg)
{
    Broadcast(msg->GetMessageType(), msg->GetExtraData());
}

void Server::ProcessObjectMessage(NetworkMessage* msg)
{
    Broadcast(msg->GetMessageType(), msg->GetExtraData());
    return;

}

void Server::InitializeNewObject(NetworkMessage* msg)
{
    Broadcast(NetworkedObjectInit, msg->GetExtraData());
}


Server::Server(std::string ip, int serverPort, IWrapper* libWrapper, LibSettings* libSettings)
{
    wrapper = libWrapper;
    settings = libSettings;
    connectingAClient = false;
    connectorInfo = nullptr;
    port = serverPort;

    ownedObjects = new std::vector<OwnedNetworkObject*>();
    nonOwnedObjects = new std::vector<UnownedNetworkObject*>();
    availableIDs = new std::queue<int>();
    availableIDs->push(1);
    connectedClients = new std::vector<EndpointInfo*>();


    address = NET_ResolveHostname(ip.c_str());
    //TODO fix blocking until address is resolved
    NET_WaitUntilResolved(address, -1);
    socket = NET_CreateDatagramSocket(address, port, 0);
    if (!socket) {
        printf("Failed to create UDP socket: %s\n", SDL_GetError());
        sender = nullptr;
        return;
    }


    sender = new ServerMessageSender(socket, connectedClients);
    StartTimer(0);
}

void Server::Update(float deltaTime)
{
    PollSocket();
    sender->SendUnsentMessages(false);
}

void Server::Broadcast(NetworkMessageTypes type, std::string message)
{
    for (int i = 0; i < connectedClients->size(); i++) {
        EndpointInfo* c = connectedClients->at(i);
        NetworkUtilities::SendMessageTo(type, message, socket, c->address, c->port, sender);
    }
}

void Server::ImportantBroadcast(NetworkMessageTypes type, std::string message)
{
    if (!sender) {
        std::cout << "Sender is null, not sending important broadcast" << std::endl;
        return;
    }
    ((ServerMessageSender*)sender)->BroadcastImportantMessage(type, message);
}

int Server::GetConnectedClientCount()
{
    return (int)connectedClients->size();
}

void Server::SendMessageTo(NetworkMessageTypes type, std::string message, EndpointInfo* receiver)
{
    NetworkUtilities::SendMessageTo(type, message, socket, address, port, sender);
}


Server::~Server()
{
	delete sender;
	NET_DestroyDatagramSocket(socket);
	for (EndpointInfo* client : *connectedClients) {
		delete client;
	}
    delete connectedClients;
    delete availableIDs;
}
