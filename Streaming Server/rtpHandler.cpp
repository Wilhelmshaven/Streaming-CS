/*--Author：李宏杰--*/

#include "rtpHandler.h"

HANDLE rtpEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::rtpEncoded);

rtpHandler* rtpHandler::instance = new rtpHandler;

rtpHandler *rtpHandler::getInstance()
{
	return instance;
}

void rtpHandler::pack(SOCKET socket, vector<int> img)
{
	string rtpPacket;
	rtpPacket.resize(sizeof(rtpOverTcpHead) + sizeof(rtpHead));

	auto aHead = (rtpOverTcpHead *)rtpPacket.c_str();

	aHead->magicNumber = '$';
	aHead->enbeddedLength = sizeof(rtpHead) + img.size();

	//信道，目前默认为1
	aHead->channelNumber = 1;

	/*
		编码：首先把vector<int>转为string，然后再在前面加上各种头部
	*/
	string imgData;
	imgData.resize(img.size());
	imgData.assign(img.begin(), img.end());

	encodeRTPHead(rtpPacket, socket);

	rtpPacket += imgData;

	packetQueue.push(rtpPacket);

	ReleaseSemaphore(rtpEncoded, 1, NULL);
}

bool rtpHandler::getPacket(string & msg)
{
	if (packetQueue.empty())
	{
		return false;
	}
	else
	{
		msg = packetQueue.front();

		packetQueue.pop();
	}

	return true;
}

unsigned short rtpHandler::getSeqNumber(SOCKET socket)
{
	unsigned short seq;

	auto iterator = seqManager.find(socket);

	/*
		逻辑：已存在记录，则取出并递增
		否则随机一个
	*/
	if (iterator != seqManager.end())
	{
		++iterator->second;

		seq = iterator->second;
	}
	else
	{
		seq = randSeq(socket);

		seqManager.insert(make_pair(socket, seq));
	}

	return seq;
}

/*
	应用<random>的算法……
*/
unsigned short rtpHandler::randSeq(SOCKET socket)
{
	//指定种子初始化随机引擎
	default_random_engine randEngine(socket);

	//随机数分布
	uniform_int_distribution<unsigned short> distribution;

	//随机……
	return distribution(randEngine);
}

void rtpHandler::encodeRTPHead(string & msg, SOCKET socket)
{
	auto rHead = (rtpHead *)(msg.c_str() + sizeof(rtpOverTcpHead));

	//10000000
	rHead->vpxcc = 0xC0;

	//10011010
	rHead->mpt = 0x9A;

	//获取序列号，查表
	rHead->seqNum = htons(getSeqNumber(socket));

	//时间戳
	rHead->timestamp = htonl(getTimeStamp(socket));

	//SSRC，随便写了……
	rHead->ssrc = htonl(0x12345678);
}

rtpHandler::rtpHandler()
{

}

unsigned long rtpHandler::getTimeStamp(SOCKET socket)
{
	auto iter = timestampManager.find(socket);

	/*
		逻辑：已存在记录，则取出并递增
		否则设初值为1
	*/
	if (iter != timestampManager.end())
	{
		++iter->second;

		return iter->second;
	}
	else
	{
		unsigned long time = 1;

		timestampManager.insert(make_pair(socket, time));

		return 1;
	}
}