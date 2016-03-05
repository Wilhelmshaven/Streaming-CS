/*--Author：李宏杰--*/

#include "rtpHandler.h"

rtpHandler *rtpHandler::instance = new rtpHandler;

rtpHandler * rtpHandler::getInstance()
{
	return instance;
}

void rtpHandler::unpackRTP(string rtpPacket)
{
	//根据格式，最简单的做法反正就是裁掉最头4个字节……
	rtpPacket = rtpPacket.substr(4, rtpPacket.length() - 4);

	vector<int> vec;

	vec.resize(rtpPacket.size());

	vec.assign(rtpPacket.begin(), rtpPacket.end());

	rtpQueue.push(vec);

	ReleaseSemaphore(hsRTPUnpacked, 1, NULL);
}

void rtpHandler::getMedia(vector<int>& img)
{
	if (!rtpQueue.empty())
	{
		img = rtpQueue.front();

		rtpQueue.pop();
	}
}

rtpHandler::rtpHandler()
{
	
}