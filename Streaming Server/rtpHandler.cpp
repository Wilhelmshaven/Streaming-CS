/*--Author������--*/

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

	//�ŵ���ĿǰĬ��Ϊ1
	aHead->channelNumber = 1;

	/*
		���룺���Ȱ�vector<int>תΪstring��Ȼ������ǰ����ϸ���ͷ��
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
		�߼����Ѵ��ڼ�¼����ȡ��������
		�������һ��
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
	Ӧ��<random>���㷨����
*/
unsigned short rtpHandler::randSeq(SOCKET socket)
{
	//ָ�����ӳ�ʼ���������
	default_random_engine randEngine(socket);

	//������ֲ�
	uniform_int_distribution<unsigned short> distribution;

	//�������
	return distribution(randEngine);
}

void rtpHandler::encodeRTPHead(string & msg, SOCKET socket)
{
	auto rHead = (rtpHead *)(msg.c_str() + sizeof(rtpOverTcpHead));

	//10000000
	rHead->vpxcc = 0xC0;

	//10011010
	rHead->mpt = 0x9A;

	//��ȡ���кţ����
	rHead->seqNum = htons(getSeqNumber(socket));

	//ʱ���
	rHead->timestamp = htonl(getTimeStamp(socket));

	//SSRC�����д�ˡ���
	rHead->ssrc = htonl(0x12345678);
}

rtpHandler::rtpHandler()
{

}

unsigned long rtpHandler::getTimeStamp(SOCKET socket)
{
	auto iter = timestampManager.find(socket);

	/*
		�߼����Ѵ��ڼ�¼����ȡ��������
		�������ֵΪ1
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