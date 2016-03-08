/*--Author：李宏杰--*/

#include "rtpHandler.h"

rtpHandler* rtpHandler::instance = new rtpHandler;

rtpHandler *rtpHandler::getInstance()
{
	return instance;
}

string rtpHandler::pack(SOCKET socket, vector<int> img)
{
	string rtpPacket;

	//1.获取序列号，查表
	short seq = getSeqNumber(socket);

	//2.信道。目前默认为一
	int channel = 1;

	/*
		3.编码

		首先把vector<int>转为string，然后再在前面加上各种头部

		先加rtp头，再加TCP的小头
	*/
	rtpPacket.resize(img.size());
	rtpPacket.assign(img.begin(), img.end());

	rtpPacket = '$' + to_string(channel) + to_string(img.size()) + rtpPacket;

	return rtpPacket;
}

short rtpHandler::getSeqNumber(SOCKET socket)
{
	short seq;

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

	return 0;
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

rtpHandler::rtpHandler()
{

}