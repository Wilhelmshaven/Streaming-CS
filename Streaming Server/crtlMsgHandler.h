/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	����˿����������������ģʽ

	�������������Ȼ�����
	����ָ�����Ӧ�м�����ݸ���Ⱦ��

	ʹ�ã�
	
	void decodeMsg(string msg)����������������������ɣ�����Ҫ���ĺ�������

*/

class ctrlMsgHandler
{
public:

	static ctrlMsgHandler* getInstance();

	void decodeMsg(int msgType, string msg);

private:

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