#pragma once
#include <SDL3_net/SDL_net.h>
#include <string>
//manages an endpoint
struct EndpointInfo {
	NET_Address* address;
	int port;
	EndpointInfo(std::string addressString, int pPort) {
		address = NET_ResolveHostname(addressString.c_str());
		NET_WaitUntilResolved(address, -1); //TODO fix timeout
		port = pPort;
	}
	EndpointInfo(NET_Address* addressRef, int pPort) {
		address = NET_RefAddress(addressRef);
		port = pPort;
	}
	~EndpointInfo() {
		NET_UnrefAddress(address);
	}
	bool operator==(const EndpointInfo& other) {
		return other.port
			== port
			&& NET_GetAddressString(other.address)
			== NET_GetAddressString(address);
	}
	bool operator!=(const EndpointInfo& other) {
		return !(*this == other);
	}
};