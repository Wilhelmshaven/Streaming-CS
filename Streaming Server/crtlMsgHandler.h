/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	����˿����������������ģʽ

	�������������Ȼ�����
	����ָ�����Ӧ�м�����ݸ���Ⱦ��

	ʹ�ã�
	
	void decodeMsg(string msg)����������������������ɣ�����Ҫ���ĺ�������

	char getKBCtrlKey()����ȡ���̲������

	char getMouseCtrlKey();��ȡ���������

*/

class ctrlMsgHandler
{
public:

	static ctrlMsgHandler* getInstance();

	void decodeMsg(int msgType, string msg);

	char getKBCtrlKey();

	char getMouseCtrlKey();

private:

	queue<char> KBQueue;

	/*
		����ģʽ
	*/

	static ctrlMsgHandler *instance;
	ctrlMsgHandler();

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
static HANDLE hsKBCtrlMsg = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);