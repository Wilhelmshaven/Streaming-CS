/*--Author������--*/

#pragma once

//I/O Headers
#include <iostream>
#include <fstream>

//STL Headers
#include <string>
#include <map>
#include <queue>
#include <list>

//Winsock Headers
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//
#include <stdlib.h>
#include <Windows.h>

//���ڽ�����ת��ΪWindows������
#include <WinUser.h>

#include <memory>

using namespace std;

namespace syncManager
{
	const char ESCPressed[] = "heESCPressed";

	const char clientClose[] = "heClientClose";

	const char playerInput[] = "hsPlayerInput";
	const char playerOutput[] = "hsPlayerOutput";

	const char bufferOutput[] = "hsBufferOutput";

	const char rtpOutput[] = "hsRTPOutput";

	const char ctrlMsgOutput[] = "hsCtrlMsgOutput";

	const char cnctInput[] = "hsCnctInput";
	const char cnctRTSPOutput[] = "hsCnctRTSPOutput";
	const char cnctRTPOutput[] = "hsCnctRTPOutput";

	const char timeOut[] = "hsTimeOut";
};

//�����ļ�
const string srvSettingFile = "config/ServerInfo.xml";             //��������Ϣ�����ļ�
const string rtspErrFile = "config/static/rtspErrCodeList.csv";    //rtsp������Ϣ

/*---------------------�Զ��峣����--------------------*/

//RTSP����
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

	//MAX��1920*1080*3
	MAX_RECV_BUF_SIZE = 6210000,

	//��ʼ֡�ʣ�10
	CLIENT_FRAME_RATE = 10,
};

/*
	������ṹ�����Ӧ�ĳ�����
*/

//�����ͷ�ṹ
typedef struct allMsgHead
{
	WORD msgSize;
	BYTE payloadType;
	BYTE cks;
	DWORD session;
};

//�����ͷ�غ������ֶα�
enum payloadType
{
	KB_MSG = 1,
	MOUSE_MSG = 2,
	IMG_MSG = 3,
};

//�������ͷ�ṹ
typedef struct mouseMsg
{
	BYTE msgType;
	BYTE relativeMode;
	BYTE isPressed;
	BYTE mouseBtn;
	WORD pointerX;
	WORD pointerY;
};

//��������ͷ�ṹ
typedef struct keyboardMsg
{
	BYTE msgType;
	BYTE isPressed;
	WORD virtualCode;
	DWORD unicode;
};

//ͼƬ����ͷ�ṹ
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
	ͼ��ͷ���������ݴ���ϵͳ�����׼ȷ����

	������

	WORD height/rows��ͼ��߶�/����

	WORD width/cols��ͼ����/����

	WORD channels��ͼ��ͨ����

	WORD imgType��ͼ�����ͣ���RGB��BGR��YUV֮�ࣨ����ЩֵĿǰδʵ�֣�

	//int matrixType���������ͣ����������Ƿ���ϡ�����Ŀǰ�����������ݣ���ʱע��δʵ�֡�
*/
typedef struct imgHead
{
	//Y�ᣬ�߶�/����
	union Y_Axis
	{
		WORD height;
		WORD rows;
	}yAxis;

	//X�ᣬ���/����
	union X_Axis
	{
		WORD width;
		WORD cols;
	}xAxis;

	//ͨ����
	WORD channels;

	//ͼ�����ͣ���RGB��BGR��YUV֮�ࣨ����ЩֵĿǰδʵ�֣�
	WORD imgType;

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
	WORD cks;
	DWORD enbeddedLength;
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