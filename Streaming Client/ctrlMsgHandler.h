/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	���������ģ�飨�ڿͻ��ˣ�Ŀǰֻ������룩
	ע�⣺Ŀǰ��ʵ�����������루���ڱ���û��Ҫ��
	��������ʵģ�����

	TODO���������Եļ��̼�λӳ���

	ʹ�ã�

	void keyboardMsgEncode(char key)�������������

	void mouseMsgEncode(char key)�������������

	bool getEncodedMsg(string &msg)����ȡ����õ�ָ��
*/

class ctrlMsgHandler
{
public:

	static ctrlMsgHandler* getInstance();

	void keyboardMsgEncode(char key);

	void mouseMsgEncode(char key);

	bool getEncodedMsg(string &msg);

private:

	queue<string> encodedMsgQueue;

	/*
		����ģʽ
	*/

	static ctrlMsgHandler *instance;

	ctrlMsgHandler();

	ctrlMsgHandler(const ctrlMsgHandler&);
	ctrlMsgHandler &operator =(const ctrlMsgHandler &);
	class CGarbo
	{
	public:
		CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo garbo;
};