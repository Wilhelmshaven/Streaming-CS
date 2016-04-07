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

namespace syncManager
{
	const char srvDown[] = "heSrvDown";

	/*
		图像源出入口信号
	*/	
	const char renderInput[] = "hsRenderInput";
	const char renderOutput[] = "hsRenderOutput";

	/*
		图像缓存出入口信号
	*/
	const char imgBufferInput[] = "hsImgBufferInput";
	const char imgBufferOutput[] = "hsImgBufferOutput";

	/*
		控制信令出入口信号：实际上是两个出口
	*/
	const char ctrlMsgEncoded[] = "hsCtrlMsgEncoded";
	const char ctrlMsgDecoded[] = "hsCtrlMsgDecoded";

	/*
		rtp包出口
	*/
	const char rtpEncoded[] = "hsRTPEncoded";

	/*
		网络模块出入口
	*/
	const char msgArrivedRTSP[] = "hsMsgArrivedRTSP";
	const char msgArrivedCtrl[] = "hsMsgArrivedCtrl";
}

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
	BUF_SIZE = 4096,
	//下面是完成端口的操作模式标记
	compRecv = 3,
	compSend = 5,
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

typedef struct rtpHead
{
	BYTE vpxcc;
	BYTE mpt;
	WORD seqNum;
	DWORD timestamp;
	DWORD ssrc;
};

typedef struct rtpOverTcpHead
{
	BYTE magicNumber;
	BYTE channelNumber;
	WORD enbeddedLength;
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
	OPENCV_MAT = 0x01,
	JPEG = 0x02,
};

enum imgMsgType
{
	IMG_HEAD = 0x01,
};