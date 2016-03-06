/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	����˿����������������ģʽ

	�������������Ȼ�����
	����ָ�����Ӧ�м�����ݸ���Ⱦ��

	ʹ�ã�
	
	void decodeMsg(string msg)������������ػỰ��

	string encodeMsg(imgHead head, int imgSize, int session)���������ͼ��ͷ��

	char getCtrlKey()����ȡ�������

*/

class ctrlMsgHandler
{
public:

	static ctrlMsgHandler* getInstance();

	int decodeMsg(string msg);

	string encodeMsg(imgHead head, int imgSize, int session);

	char getCtrlKey();

private:

	//���빫��ͷ
	string encodePublicHead(int payloadType, int session, int size);

	queue<char> ctrlKeyQueue;

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

//���������ģ�飺������������ϣ����м������ת����Ⱦ��
static HANDLE hsCtrlMsg = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);