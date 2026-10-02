#include "TestingFunctions.h"
void SendDummyMessage(std::string toAddress, int toPort, std::string msg, std::string fromAddress, int fromPort) {
	NET_Address* fromAddr = NET_ResolveHostname(fromAddress.c_str());
	NET_WaitUntilResolved(fromAddr, -1);
	NET_Address* toAddr = NET_ResolveHostname(toAddress.c_str());
	NET_WaitUntilResolved(toAddr, -1);
	NET_DatagramSocket* socket = NET_CreateDatagramSocket(fromAddr, fromPort, 0);
	const void* buf = (void*)msg.data();
	const int buflen = msg.length();
	NET_SendDatagram(socket, toAddr, toPort, buf, buflen);
	NET_UnrefAddress(fromAddr);
	NET_UnrefAddress(toAddr);
	NET_DestroyDatagramSocket(socket);
}