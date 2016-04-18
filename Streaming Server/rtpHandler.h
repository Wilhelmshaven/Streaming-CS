/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	RTP�࣬���Ǹ��������
	��������ý�����ݣ����������

	Usage��

	bool pack(SOCKET socket, imgHead head, shared_ptr<vector<BYTE>> img)�������׽�����ͼ�����ݱ���RTP�����������û�н��������Ѿ���Teardown����ôֱ�ӷ��ط�

	bool getPacket(SOCKET &index, shared_ptr<string> &msg)��ȡ��RTP��

*/
class rtpHandler
{
public:

	static rtpHandler* getInstance();

	//�����׽�����ͼ�����ݱ���RTP�����������û�н��������Ѿ���Teardown����ôֱ�ӷ��ط�
	bool pack(SOCKET socket, imgHead head, shared_ptr<vector<BYTE>> img);

	//ȡ��RTP��
	bool getPacket(SOCKET &index, shared_ptr<string> &msg);

private:

	typedef struct myPacket
	{
		SOCKET index;
		shared_ptr<string> packet;
	};

	//Ϊ�˷�����
	queue<myPacket> packetQueue;

	/*
		rtp���еİ����кŹ�����
	*/
	map<SOCKET, unsigned short> seqManager;

	unsigned short getSeqNumber(SOCKET socket);

	//������к���������ʹ�ñ�׼��ʵ��
	unsigned short randSeq(SOCKET socket);

	/*
		�������ͷ��
	*/
	void encodeRTPTCPHead(string &msg, size_t size);

	void encodeRTPHead(string &msg, SOCKET socket);

	void encodeImgHead(string &msg, imgHead head);

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