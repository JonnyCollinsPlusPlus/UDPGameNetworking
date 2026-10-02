#pragma once
#include <string>
#include <SDL3_net/SDL_net.h>
#include <bitset>
#include <iostream>
#include <vector>
#include "ImportantMessage.h"
#include "UDPGameNetworkingAPI.h"
//Forward declaration
class MessageSender;
//Different types of message that can be sent
//Note message importance is defined in NetworkUtilities::IsImportantType();
enum NetworkMessageTypes {
	Connect,
	ConnectConfirm,
	NetworkedObjectMsg,
	NetworkedObjectInit,
	UserImportant,
	UserUnImportant,
	ImportantMessageConfirmation,
	IDRequest,
	InvokeCallback,
	Error
};

class UDPGAMENETWORKING_API NetworkUtilities {
private:
	//TODO documentation expansion
	friend class MessageSender;
	//Packs message data into a buffer of unsigned ints to be sent
	static std::vector<Uint8>* PackMessage(std::string inData);
	//convert a message type into its corresponding header
	static std::string PackHeader(NetworkMessageTypes type);
	//Here we declare which messages are important
	// all messages received of these type will trigger an important message response
	static bool IsImportantType(NetworkMessage* message);
	static bool IsImportantType(NetworkMessageTypes type);
	static void SendMessageDirect(NetworkMessageTypes messageType, std::string message, NET_DatagramSocket* socket, NET_Address* address, int port);
public:
	//Gets the next incoming message on the socket
	//Implementation should look something like this:
	//NetworkMessage* message = nullptr;
	//while(GetNextIncoming(socket, message){
	//process data and message here
	//delete message; }
	// @param socket - the SDLNet datagram socket to poll for new messages
	// @param message - the message pointer to populate with a new message
	// @param sender - the message sender to process important messages with, default is nullptr
	// @return bool if there were any new message*/
	static bool GetNextIncoming(NET_DatagramSocket* socket, NetworkMessage*& message, MessageSender* sender = nullptr);
	//Sends a message over UDP
	// @param messageType - the header of the message, note that this determines if message is important or not
	// @param message - the message data to send, note that this must be binary only
	// @param socket - the socket to send the message from
	// @param address - the address to send the message to
	// @param port - the port to send the message to on the specified address
	// @param sender - the message sender to send important messages with, default is nullptr
	static void SendMessageTo(NetworkMessageTypes messageType, std::string message, NET_DatagramSocket* socket, NET_Address* address, int port, MessageSender* sender = nullptr);
	//Verify that a string contains only '1' and '0' characters
	// @param message - string to query
	// @return bool wether or not the string was comprised of only 1s and 0s
	static bool IsBinaryOnly(std::string message);
	//Unpacks a Uint8* into a string of '1' and '0' characters up to the specified message length
	// @param inData - the Uint8* received from the incomingMessage
	// @param messageLength - the length of data to unpack (must not exceed Uint8* size)
	// @return returns the unpacked message as a string of 1s and 0s
	static std::string UnpackMessage(Uint8* inData, int messageLength);

	static NetworkMessageTypes UnpackHeader(std::string message);

	static bool VeryifyMessageState(NetworkMessage* msg, std::string stateCode);


	//converts a decimal integer into a binary coded decimal string
	// @param outNibbles - the number of nibbles (4 bits) to return, any value that can not be shown in this length will be ignored
	// @param value - the integer to convert into BCD
	static std::string AsBinaryString(int value, int digits);
	//Converts a binary string into an integer
	// @param binaryString - the string to convert from (note that BCD is assumed and that the string will be read from the start
	// @param digits - the number of BCD digits to read (each digit is 4 bits) note that if the string is longer then only the start will be read
	static int IntFromBinaryString(std::string binaryString, int digits);

	static std::string AsBinaryString(std::string value, int outBits);
	static std::string StringFromBinaryString(std::string binaryString, int length);

};