/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	��Ϣ�м��
	
	ʹ�ã�

	������ȵ���startMiddleWare()������ģ��
	�ر������ڷ������ر��¼�
*/

class mwMsg
{
public:

	static mwMsg* getInstance();

	void startMiddleWare();

private:

	static DWORD WINAPI mwCtrlMsgThread(LPVOID lparam);

	static DWORD WINAPI mwRTSPMsgThread(LPVOID lparam);

	/*
		����ģʽ
	*/

	mwMsg();

	static mwMsg* instance;
	
	mwMsg(const mwMsg&);
	mwMsg &operator=(const mwMsg&);
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