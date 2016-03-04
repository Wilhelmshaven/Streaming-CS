/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	���������ģ�飨�ڿͻ��ˣ�Ŀǰֻ������룩
	ע�⣺Ŀǰ��ʵ�����������루���ڱ���û��Ҫ��
	��������ʵģ�����

	TODO���������Եļ��̼�λӳ���

	ʹ�ã�

	string keyboardMsgEncode()�������������

	string mouseMsgEncode()�������������
*/

class msgHandler
{
public:

	static msgHandler* getInstance();

	string keyboardMsgEncode(char key);

	string mouseMsgEncode();

private:

	/*
		����ģʽ
	*/

	static msgHandler *instance;

	msgHandler();

	msgHandler(const msgHandler&);
	msgHandler &operator =(const msgHandler &);
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