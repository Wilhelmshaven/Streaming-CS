/*--Author：李宏杰--*/

#pragma once

//I/O Headers
#include <iostream>
#include <fstream>

//STL Headers
#include <string>
#include <map>
#include <queue>

//Winsock Headers
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//
#include <stdlib.h>
#include <Windows.h>

//用于将输入转换为Windows虚拟码
#include <WinUser.h>

using namespace std;

namespace syncManager
{
	const char clientClose[] = "heClientClost";

	const char playerInput[] = "hsPlayerInput";
	const char playerOutput[] = "hsPlayerOutput";

	const char bufferOutput[] = "hsBufferOutput";

	const char rtpOutput[] = "hsRTPOutput";

	const char ctrlMsgOutput[] = "hsCtrlMsgOutput";

	const char cnctInput[] = "hsCnctInput";
	const char cnctRTSPOutput[] = "hsCnctRTSPOutput";
	const char cnctRTPOutput[] = "hsCnctRTPOutput";
};

//配置文件
const string srvSettingFile = "config/ServerInfo.xml";             //服务器信息配置文件
const string rtspErrFile = "config/static/rtspErrCodeList.csv";    //rtsp错误消息

/*---------------------自定义常量区，少的话这里写，多的话，不如用map，例如rtsp错误信息--------------------*/
//服务器自定义结构体中涉及的服务器信息，但全局可用
//类：cnctHandler
enum ServerInfoDefine
{
	srvRsv = 0,        //默认值，保留！
	srvType = 1,       //服务器类型
	protocol = 2,      //地址头
	hostName = 3,      //域名
	port = 4,          //端口
	displayRoute = 5,  //播放地址
	hostAddr = 6,      //额，IP地址
	ServerArgc = 7,    //这个放在最后，表示参数的数量
};

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
};

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

//图片信令头结构
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

typedef struct imgMsgCombine
{
	allMsgHead publicHead;
	imgMsgHead imgHead;
};

/*
	图像头，内容依据传输系统信令标准确定。

	变量：

	WORD height/rows：图像高度/行数

	WORD width/cols：图像宽度/列数

	WORD channels：图像通道数

	WORD imgType：图像类型，如RGB、BGR、YUV之类（但这些值目前未实现）

	//int matrixType：矩阵类型，用于区分是否是稀疏矩阵。目前都传完整数据，暂时注释未实现。
*/
typedef struct imgHead
{
	union Y_Axis
	{
		WORD height;
		WORD rows;
	}yAxis;

	union X_Axis
	{
		WORD width;
		WORD cols;
	}xAxis;

	WORD channels;

	//图像类型，如RGB、BGR、YUV之类（但这些值目前未实现）
	WORD imgType;

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