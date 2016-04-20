/*--Author：李宏杰--*/

#include "rtpHandler.h"

//
#include "clientManager.h"

HANDLE rtpEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::rtpEncoded);

rtpHandler* rtpHandler::instance = new rtpHandler;

clientManager* myClientList = clientManager::getInstance();

rtpHandler *rtpHandler::getInstance()
{
	return instance;
}

/*
	RTP包打包结构

	TCP的RTP头+RTP头+图像头+图像数据

	一张图一个RTP包 -> 这是基于TCP的特点确定的

	如果连接没有建立，或者已经被Teardown，那么直接返回否
*/
bool rtpHandler::pack(SOCKET socket, imgHead head, shared_ptr<vector<BYTE>> img)
{
	/*
		!!
		这里检查是否已被Teardown
		这里不写的话，就必须得在其它地方实现，所以还是写在这里方便s
	*/
	if (!myClientList->searchClient(socket))
	{
		return false;
	}

	size_t headSize = sizeof(rtpOverTcpHead) + sizeof(rtpHead) + sizeof(imgHead);
	size_t imageSize = (*img).size();

	shared_ptr<string> rtpPacket(new string);

	(*rtpPacket).resize(headSize + imageSize);

	/*
		编码：首先把vector<BYTE>转为string，然后再在前面加上各种头部
	*/

	encodeRTPTCPHead((*rtpPacket), headSize + imageSize);

	encodeRTPHead((*rtpPacket), socket);

	encodeImgHead((*rtpPacket), head);

	memcpy(&(((*rtpPacket))[headSize]), &((*img)[0]), imageSize);

	shared_ptr<myPacket> packet(new myPacket);
	(*packet).index = socket;
	(*packet).packet = rtpPacket;

	packetQueue.push(packet);

	ReleaseSemaphore(rtpEncoded, 1, NULL);

	return true;
}

bool rtpHandler::getPacket(SOCKET &index, shared_ptr<string> & msg)
{
	if (packetQueue.empty())
	{
		return false;
	}

	shared_ptr<myPacket> packet = packetQueue.front();

	packetQueue.pop();

	index = (*packet).index;
	msg = (*packet).packet;

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

void rtpHandler::encodeRTPTCPHead(string & msg, size_t size)
{
	auto aHead = (rtpOverTcpHead *)msg.c_str();

	aHead->magicNumber = '$';

	aHead->enbeddedLength = htonl(size);

	aHead->cks = 0;

	//信道，目前默认为1
	aHead->channelNumber = 1;
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

void rtpHandler::encodeImgHead(string & msg, imgHead head)
{
	auto iHead = (imgHead *)(msg.c_str() + sizeof(rtpOverTcpHead) + sizeof(rtpHead));

	iHead->channels = htons(head.channels);

	iHead->cols = htons(head.cols);

	iHead->rows = htons(head.rows);

	iHead->imgType = htons(head.imgType);
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

	unsigned long time = 1;

	timestampManager.insert(make_pair(socket, time));

	return 1;
}