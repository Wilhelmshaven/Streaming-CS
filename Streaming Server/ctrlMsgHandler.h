/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	����˿����������������ģʽ

	�������������Ȼ�����
	����ָ�����Ӧ�м�����ݸ���Ⱦ��

	ʹ�ã�
	
	void decodeMsg(string msg)����������������������ɣ�����Ҫ���ĺ�������

	char getCtrlKey()����ȡ�������

*/

class ctrlMsgHandler
{
public:

	static ctrlMsgHandler* getInstance();

	void decodeMsg(string msg);

	char getCtrlKey();

private:

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