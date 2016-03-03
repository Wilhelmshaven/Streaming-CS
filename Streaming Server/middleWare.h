/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	������ṹ�����Ӧ�ĳ�����
*/

typedef struct allMsgHead
{
	WORD msgSize;
	BYTE payloadType;
	BYTE cks;
	DWORD session;
};

enum payloadType
{
	KB_MSG = 1,
	MOUSE_MSG = 2,
	IMG_MSG = 3,
};

typedef struct mouseMsg
{
	BYTE msgType;
	BYTE relativeMode;
	BYTE isPressed;
	BYTE mouseBtn;
	WORD pointerX;
	WORD pointerY;
};

typedef struct keyboardMsg
{
	BYTE msgType;
	BYTE isPressed;
	WORD virtualCode;
	DWORD unicode;
};

typedef struct imgHead
{
	WORD msgSize;
	BYTE msgType;
	BYTE msgFlag;
	WORD imgRows;
	WORD imgCols;
	WORD imgChannels;
	BYTE imgType;
	BYTE payloadType;
};

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

	static mwMsg* instance;
	mwMsg();

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