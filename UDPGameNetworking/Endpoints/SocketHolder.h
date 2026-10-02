#pragma once
#include "../NetworkUtilities.h"
#include "../Senders/MessageSender.h"
#include "../UDPGameNetworkingAPI.h"
class IWrapper; //forward declaration
class UDPGAMENETWORKING_API SocketHolder {
private:
	int timerOffset = 0;
protected:
	IWrapper* wrapper;
	NET_DatagramSocket* socket;
	void PollSocket();
	virtual void ProcessMessage(NetworkMessage* msg) = 0;
	virtual MessageSender* GetSender() = 0;
	void StartTimer(int serverTime);
	bool TimerStarted();
public:
	int GetTime();
};