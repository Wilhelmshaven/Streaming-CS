/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

enum keyboardLayout
{
	KB_ENG = 1,
};

enum keyboardState
{
	KB_PRESS = 1,
	KB_RELEASE = 2,
	KB_CLICK = 3,
};

/*
	���������ģ�飨�ڿͻ��ˣ�Ŀǰֻ������룩
	ע�⣺Ŀǰ��ʵ�����������루���ڱ���û��Ҫ��
	��������ʵģ�����

	TODO���������Եļ��̼�λӳ���
*/

class msgHandler
{
public:

	static msgHandler* getInstance();

	string ctrlMsgEncode();

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