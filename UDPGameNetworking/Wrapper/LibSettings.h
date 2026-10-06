#pragma once
#include "../UDPGameNetworkingAPI.h"

struct UDPGAMENETWORKING_API LibSettings {
private:
protected:
public:
	int lerpDelay;
	bool lerpEnabled;
	bool ghostEnabled;
	int ackResendDelay;
};