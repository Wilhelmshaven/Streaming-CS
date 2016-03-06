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
	string pack(SOCKET socket, vector<int> img);

private:

	/*
		���кŹ�����
	*/
	map<SOCKET, unsigned short> seqManager;

	short getSeqNumber(SOCKET socket);

	//������к���������ʹ�ñ�׼��ʵ��
	unsigned short randSeq(SOCKET socket);

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