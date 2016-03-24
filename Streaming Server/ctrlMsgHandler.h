/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	����˿����������������ģʽ

	�������������Ȼ�����
	����ָ�����Ӧ�м�����ݸ���Ⱦ��

	ʹ�ã�
	
	void decodeMsg(string msg)����������

	void encodeMsg(imgHead head, int imgSize, int session)����������

	bool getDecodedMsg(int &session, char &ctrlKey)��ȡ��������

	bool getEncodedMsg(string &encodedMsg)��ȡ��������

*/

class ctrlMsgHandler
{
public:

	static ctrlMsgHandler* getInstance();

	void decodeMsg(string msg);

	void encodeMsg(imgHead head, int imgSize, int session);

	bool getDecodedMsg(unsigned int &session, char &ctrlKey);

	bool getEncodedMsg(string &encodedMsg);

private:

	typedef struct decodedMsg
	{
		int session;

		char ctrlKey;
	};

	queue<decodedMsg> decodedMsgQueue;

	queue<string> encodedMsgQueue;
	
	//���빫��ͷ
	string encodePublicHead(int payloadType, int session, int size);

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