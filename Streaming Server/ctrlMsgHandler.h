/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	����˿����������������ģʽ

	�������������Ȼ�����
	����ָ�����Ӧ�м�����ݸ���Ⱦ��

	ʹ�ã�
	
	void decodeMsg(string msg)����������

	//void encodeMsg(imgHead head, unsigned int imgSize, unsigned int session)����������

	bool getDecodedMsg(unsigned int &session, unsigned char &ctrlKey)��ȡ��������

	//bool getEncodedMsg(string &encodedMsg)��ȡ��������

*/
class ctrlMsgHandler
{
public:

	static ctrlMsgHandler* getInstance();

	void decodeMsg(SOCKET index, string msg);

	//void encodeMsg(imgHead head, unsigned int imgSize, unsigned int session);

	bool getDecodedMsg(SOCKET &index, unsigned int &session, unsigned char &ctrlKey);

	//bool getEncodedMsg(string &encodedMsg);

private:

	typedef struct decodedMsg
	{
		SOCKET index;

		unsigned int session;

		unsigned char ctrlKey;
	};

	queue<decodedMsg> decodedMsgQueue;

	//queue<string> encodedMsgQueue;
	
	//���빫��ͷ
	string encodePublicHead(unsigned int payloadType, unsigned int session, unsigned int size);

	/*
		����ģʽ
	*/

	ctrlMsgHandler();

	static ctrlMsgHandler *instance;
	
	ctrlMsgHandler(const ctrlMsgHandler&);
	ctrlMsgHandler &operator=(const ctrlMsgHandler&);
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