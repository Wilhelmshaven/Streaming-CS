#pragma once

//I/O Headers
#include <iostream>
#include <fstream>

// STL Headers
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <random>

// Winsock Headers
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//
#include <stdlib.h>

#include <WinUser.h>

using namespace std;

/*---------------------自定义常量区，少的话这里写，多的话，不如用map，例如rtsp错误信息--------------------*/
//RTSP方法
enum rtspMethod
{
	rtspRsv = 0,
	OPTIONS = 1,
	DESCRIBE = 2,
	SETUP = 3,
	PLAY = 4,
	GET_PARAMETER = 5,
	PAUSE = 6,
	TEARDOWN = 7,
	ANNOUNCE = 8,
	RECORD = 9,
	REDIRECT = 10,
	SET_PARAMETER = 11,
	rtspMethodArgs = 12,
};

enum CustomDefine
{
	BUF_SIZE = 8192,
	//下面是完成端口的操作模式标记
	compRecv = 3,
	compSend = 5,
};

//全局事件
static HANDLE hSrvShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);    //结束服务器的事件！！

static HANDLE hsMsgHandler = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);
static HANDLE hsCamCap = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);

/*
	EXTERN 信号量

	各模块的信号量写在各模块的头部，这里只是为了方便使用

	不在这里声明是因为这样违反组件化规则……
*/

//网络模块：标记有新的RTSP信令信息来到
extern HANDLE hsRTSPMsgArrived;

//网络模块：标记有新的控制信令信息来到
extern HANDLE hsCtrlMsgArrived;

//流媒体信令处理模块：标记有新的播放/停止播放请求，请RTP模块拿走会话号
extern HANDLE hsPlaySession;
extern HANDLE hsStopSession;

//控制信令处理模块：标记信令解码完毕，请中间件拿走转给渲染器
extern HANDLE hsCtrlMsg;

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

typedef struct imgMsgHead
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
