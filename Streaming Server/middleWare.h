/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	各信令结构体与对应的常量表
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
	消息中间件
	
	使用：

	请务必先调用startMiddleWare()启动本模块
	关闭依赖于服务器关闭事件
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
		单例模式
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