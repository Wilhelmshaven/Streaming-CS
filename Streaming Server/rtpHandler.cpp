/*--Author������--*/

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
	RTP������ṹ

	TCP��RTPͷ+RTPͷ+ͼ��ͷ+ͼ������

	һ��ͼһ��RTP�� -> ���ǻ���TCP���ص�ȷ����

	�������û�н����������Ѿ���Teardown����ôֱ�ӷ��ط�
*/
bool rtpHandler::pack(SOCKET socket, imgHead head, vector<unsigned char> img)
{
	/*
		!!
		�������Ƿ��ѱ�Teardown
		���ﲻд�Ļ����ͱ�����������ط�ʵ�֣����Ի���д�����﷽��s
	*/
	if (!myClientList->searchClient(socket))
	{
		return false;
	}

	size_t headSize = sizeof(rtpOverTcpHead) + sizeof(rtpHead) + sizeof(imgHead);

	string rtpPacket;

	rtpPacket.resize(headSize + img.size());

	/*
		���룺���Ȱ�vector<unsigned char>תΪstring��Ȼ������ǰ����ϸ���ͷ��
	*/

	encodeRTPTCPHead(rtpPacket, headSize + img.size());

	encodeRTPHead(rtpPacket, socket);

	encodeImgHead(rtpPacket, head);

	memcpy(&rtpPacket[headSize], &img[0], img.size());

	myPacket packet;
	packet.index = socket;
	packet.packet = rtpPacket;

	packetQueue.push(packet);

	ReleaseSemaphore(rtpEncoded, 1, NULL);

	return true;
}

bool rtpHandler::getPacket(SOCKET &index, string & msg)
{
	if (packetQueue.empty())
	{
		return false;
	}

	myPacket packet = packetQueue.front();

	packetQueue.pop();

	index = packet.index;
	msg = packet.packet;

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

void rtpHandler::encodeRTPTCPHead(string & msg, size_t size)
{
	auto aHead = (rtpOverTcpHead *)msg.c_str();

	aHead->magicNumber = '$';

	aHead->enbeddedLength = size;

	//�ŵ���ĿǰĬ��Ϊ1
	aHead->channelNumber = 1;
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

void rtpHandler::encodeImgHead(string & msg, imgHead head)
{
	auto iHead = (imgHead *)(msg.c_str() + sizeof(rtpOverTcpHead) + sizeof(rtpHead));

	iHead->channels = head.channels;

	iHead->cols = head.cols;

	iHead->rows = head.rows;

	iHead->imgType = head.imgType;
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

	unsigned long time = 1;

	timestampManager.insert(make_pair(socket, time));

	return 1;
}