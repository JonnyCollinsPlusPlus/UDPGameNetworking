#pragma once
#include "NetworkedValue.h"
#include "../UDPGameNetworkingAPI.h"
#include <vector>
struct LibSettings;
struct Position {
private:
protected:
public:
	Position(int xVal, int yVal);
	int x;
	int y;
};
struct PositionDataPoint {
private:
protected:
public:
	PositionDataPoint(int xVal, int yVal, int timeVal);
	int x;
	int y;
	int time;
};
class UDPGAMENETWORKING_API PositionLerp2D : public NetworkedValue<Position*> {
private:
	//for use when streaming out (I.E owned object)
	int x;
	int y;
	//for use when streaming in (I.E. non owned object)
	std::vector<PositionDataPoint*>* dataBuffer;
	//marks a new message being received
	void LerpMessageReceived(Position* pos, int time);

protected:
public:
	PositionLerp2D(int ID, int initX = 0, int initY = 0);
	const inline int GetX() { return clampToScreenSize(x); }
	const inline int GetY() { return clampToScreenSize(y); }

	virtual bool StreamReceived(std::string streamData, int time) override;

	virtual int GetPacketPayloadLength() override;

	void UpdateValue(int xVal, int yVal);

	virtual std::string Debug() override;

	static int clampToScreenSize(int clampVal);

	virtual Position* Deserialize(std::string) override;

	virtual std::string Serialize(Position*);

	virtual Position* GetCurrentValue(int currentTime, LibSettings* settings) override;

	virtual std::string GetMetadata() override;

	Position* GetMostRecentValue(); //For rendering in demo
};