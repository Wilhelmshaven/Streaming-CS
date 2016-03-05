/*--Author������--*/

#include "rtpHandler.h"

rtpHandler *rtpHandler::instance = new rtpHandler;

rtpHandler * rtpHandler::getInstance()
{
	return instance;
}

void rtpHandler::unpackRTP(string rtpPacket)
{
	//���ݸ�ʽ����򵥵������������ǲõ���ͷ4���ֽڡ���
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