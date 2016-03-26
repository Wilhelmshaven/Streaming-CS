/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	RTP�࣬���Ǹ��������
	��������ý�����ݣ����������

	Usage��

	string pack(SOCKET socket, vector<int> img)�������׽�����ͼ�����ݣ����ر���õ�RTP��

*/

class rtpHandler
{
public:

	static rtpHandler* getInstance();

	//�����׽�����ͼ�����ݣ����ر���õ�RTP��
	void pack(SOCKET socket, vector<int> img);

	bool getPacket(string &msg);

private:

	//Ϊ�˷�����
	queue<string> packetQueue;

	/*
		rtp���еİ����кŹ�����
	*/
	map<SOCKET, unsigned short> seqManager;

	unsigned short getSeqNumber(SOCKET socket);

	//������к���������ʹ�ñ�׼��ʵ��
	unsigned short randSeq(SOCKET socket);

	void encodeRTPHead(string &msg, SOCKET socket);

	map<SOCKET, unsigned long> timestampManager;

	unsigned long getTimeStamp(SOCKET socket);

	/*
		����
	*/
	static rtpHandler* instance;

	rtpHandler();

	rtpHandler(const rtpHandler&);
	rtpHandler &operator=(const rtpHandler &);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo garbo;
};