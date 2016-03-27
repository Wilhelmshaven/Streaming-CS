/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	��Ϣ�м��
	
	ʹ�ã�

	������ȵ���startMiddleWare()������ģ��
	�ر������ڷ������ر��¼�
*/

class middleWare
{
public:

	static middleWare* getInstance();

	void startMiddleWare();

private:

	void initHandles();

	static DWORD WINAPI mwCtrlMsgThread(LPVOID lparam);

	static DWORD WINAPI mwRTSPMsgThread(LPVOID lparam);

	/*
		����ģʽ
	*/

	middleWare();

	static middleWare* instance;
	
	middleWare(const middleWare&);
	middleWare &operator=(const middleWare&);
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