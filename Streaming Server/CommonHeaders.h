/*--Author：李宏杰--*/

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
	BUF_SIZE = 2048,
	//下面是完成端口的操作模式标记
	compRecv = 3,
	compSend = 5,
};

//全局事件
static HANDLE hSrvShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);    //结束服务器的事件！！

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

//摄像头：标记中间件已拿到并转发解码好的指令，请渲染器（摄像头）处理
extern HANDLE hsRender;

//图像缓存：标记图像已渲染好，请中间件拿走
extern HANDLE hsImageReady;

/*
	各信令结构体与对应的常量表
*/

//信令公共头结构
typedef struct allMsgHead
{
	WORD msgSize;         
	BYTE payloadType;
	BYTE cks;
	DWORD session;
};

//信令公共头载荷类型字段表
enum payloadType
{
	KB_MSG = 1,
	MOUSE_MSG = 2,
	IMG_MSG = 3,
};

//鼠标信令头结构
typedef struct mouseMsg
{
	BYTE msgType;
	BYTE relativeMode;
	BYTE isPressed;
	BYTE mouseBtn;
	WORD pointerX;
	WORD pointerY;
};

//键盘信令头结构
typedef struct keyboardMsg
{
	BYTE msgType;
	BYTE isPressed;
	WORD virtualCode;
	DWORD unicode;
};

//图片头结构
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

/*
	图像头，内容依据传输系统信令标准确定。

	变量：

	int height/rows：图像高度/行数

	int width/cols：图像宽度/列数

	int channels：图像通道数

	int imgType：图像类型，如RGB、BGR、YUV之类（但这些值目前未实现）

	//int matrixType：矩阵类型，用于区分是否是稀疏矩阵。目前都传完整数据，暂时注释未实现。
*/
typedef struct imgHead
{
	union
	{
		int height;
		int rows;
	};

	union
	{
		int width;
		int cols;
	};

	int channels;

	//图像类型，如RGB、BGR、YUV之类（但这些值目前未实现）
	int imgType;

	//int matrixType;
};

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

enum imgPayloadType
{
	OPENCV_MAT = 1,
	JPEG = 2,
};

enum imgMsgType
{
	IMG_HEAD = 1,
};