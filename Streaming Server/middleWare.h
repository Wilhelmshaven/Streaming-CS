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
	消息中间件：
	从网络模块中取出消息，预处理后转给RTSP处理模块或控制信令处理模块
	从RTSP处理模块中取出编码结果，交给网络模块
*/

class mwMsg
{
public:
	static mwMsg* getInstance();

	//统一的入口，返回值为0则表示不需要继续处理，否则表示还需要调用msgOut()方法获取回发需要的信令
	int msgIn(string msg);

	string msgOut();

private:



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