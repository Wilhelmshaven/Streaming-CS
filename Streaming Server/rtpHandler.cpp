/*--Author������--*/

#include "rtpHandler.h"

rtpHandler* rtpHandler::instance = new rtpHandler;

rtpHandler *rtpHandler::getInstance()
{
	return instance;
}

string rtpHandler::pack(SOCKET socket, vector<int> img)
{
	string rtpPacket;

	//1.��ȡ���кţ����
	short seq = getSeqNumber(socket);

	//2.�ŵ���ĿǰĬ��Ϊһ
	int channel = 1;

	/*
		3.����

		���Ȱ�vector<int>תΪstring��Ȼ������ǰ����ϸ���ͷ��

		�ȼ�rtpͷ���ټ�TCP��Сͷ
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

	return 0;
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

rtpHandler::rtpHandler()
{

}