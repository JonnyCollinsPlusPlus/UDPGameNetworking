#include "PositionLerp2D.h"
#include "../NetworkUtilities.h"
int PositionLerp2D::clampToScreenSize(int clampVal)
{
    int screenSize = 512;
    return ((clampVal % screenSize) + screenSize) % screenSize;
}
Position* PositionLerp2D::Deserialize(std::string data)
{
    if (data.size() != GetPacketPayloadLength()) return nullptr;
    int xIn = NetworkUtilities::IntFromBinaryString(data.substr(0, 32), 8);
    int yIn = NetworkUtilities::IntFromBinaryString(data.substr(32, 32), 8);
    return new Position(xIn, yIn);
}
std::string PositionLerp2D::Serialize(Position*)
{
    std::string streamData = "";
    streamData.append(NetworkUtilities::AsBinaryString(GetX(), 8));
    streamData.append(NetworkUtilities::AsBinaryString(GetY(), 8));
    return streamData;
}
Position* PositionLerp2D::GetCurrentValue(int currentTime, LibSettings* settings)
{
    if (!settings->lerpEnabled) {
        while (dataBuffer->size() > 1) {
            dataBuffer->erase(dataBuffer->begin());
        }
        if (dataBuffer->size() > 0) {
            x = dataBuffer->at(0)->x;
            y = dataBuffer->at(0)->y;
        }
        return new Position(x, y);
    }
    currentTime -= settings->lerpDelay;
    //only interpolate if there are more than 2 elements in the buffer
    if (dataBuffer->size() >= 2) {
        if (dataBuffer->at(1)->time < currentTime) {
            dataBuffer->erase(dataBuffer->begin());
            //if the time is past the second element of incomingDataBuffer, then we dont need the first element for interpolation anymore so we can remove it
        }
        //check again if the data bufer has more than 2 elements, one may have been deleted since the first check
        if (dataBuffer->size() >= 2) {
            //calculate the duration of the snapshot (the time between the first and second data elements)
            double snapshotDuration = (dataBuffer->at(1)->time - dataBuffer->at(0)->time);
            //calculate time since the start of the snapshot
            double snapshotSinceStart = (currentTime - dataBuffer->at(0)->time);
            //calculate progress through this snapshot
            double snapshotProgress = snapshotSinceStart / snapshotDuration;
            if (snapshotProgress > 1) { //a new packet has taken too long to arrive, no data is available for where the object should currently be so just stay still
                x = clampToScreenSize(dataBuffer->at(1)->x);
                y = clampToScreenSize(dataBuffer->at(1)->y);
            }
            else { //in the range of being able to interpolate
                x = clampToScreenSize(dataBuffer->at(0)->x) + ((clampToScreenSize(dataBuffer->at(1)->x) - clampToScreenSize(dataBuffer->at(0)->x)) * snapshotProgress);
                y = clampToScreenSize(dataBuffer->at(0)->y) + ((clampToScreenSize(dataBuffer->at(1)->y) - clampToScreenSize(dataBuffer->at(0)->y)) * snapshotProgress);
            }
        }
    }
    return new Position(x, y);
}
std::string PositionLerp2D::GetMetadata()
{
    return "00000000";
}
PositionLerp2D::PositionLerp2D(int ID, int initX, int initY) : NetworkedValue(ID)
{
	x = initX;
	y = initY;
	dataBuffer = new std::vector<PositionDataPoint*>();
}

void PositionLerp2D::LerpMessageReceived(Position* pos, int time)
{
	dataBuffer->push_back(new PositionDataPoint(pos->x, pos->y, time));
}

bool PositionLerp2D::StreamReceived(std::string streamData, int time)
{
    Position* newPos = Deserialize(streamData);
    if (!newPos) {
        delete newPos;
        newPos = nullptr;
        return false;
    }
	LerpMessageReceived(newPos, time);
    delete newPos;
    newPos = nullptr;
	return true;
}


int PositionLerp2D::GetPacketPayloadLength()
{
    return 64;
}

void PositionLerp2D::UpdateValue(int xVal, int yVal)
{
	x = xVal;
	y = yVal;
}

Position* PositionLerp2D::GetMostRecentValue(){
    PositionDataPoint* pdp = dataBuffer->back();
    return new Position(pdp->x,pdp->y);
}

std::string PositionLerp2D::Debug()
{
	return "position lerp with x: " + std::to_string(x) + " and y: " + std::to_string(y);
}

PositionDataPoint::PositionDataPoint(int xVal, int yVal, int timeVal)
{
	x = xVal;
	y = yVal;
	time = timeVal;
}

Position::Position(int xVal, int yVal)
{
	x = xVal;
	y = yVal;
}
