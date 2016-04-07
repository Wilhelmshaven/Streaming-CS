/*--Author������--*/

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
		ͼ��Դ������ź�
	*/	
	const char renderInput[] = "hsRenderInput";
	const char renderOutput[] = "hsRenderOutput";

	/*
		ͼ�񻺴������ź�
	*/
	const char imgBufferInput[] = "hsImgBufferInput";
	const char imgBufferOutput[] = "hsImgBufferOutput";

	/*
		�������������źţ�ʵ��������������
	*/
	const char ctrlMsgEncoded[] = "hsCtrlMsgEncoded";
	const char ctrlMsgDecoded[] = "hsCtrlMsgDecoded";

	/*
		rtp������
	*/
	const char rtpEncoded[] = "hsRTPEncoded";

	/*
		����ģ������
	*/
	const char msgArrivedRTSP[] = "hsMsgArrivedRTSP";
	const char msgArrivedCtrl[] = "hsMsgArrivedCtrl";
}

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
	BUF_SIZE = 4096,
	//��������ɶ˿ڵĲ���ģʽ���
	compRecv = 3,
	compSend = 5,
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

//ͼƬͷ�ṹ
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
	ͼ��ͷ���������ݴ���ϵͳ�����׼ȷ����

	������

	int height/rows��ͼ��߶�/����

	int width/cols��ͼ����/����

	int channels��ͼ��ͨ����

	int imgType��ͼ�����ͣ���RGB��BGR��YUV֮�ࣨ����ЩֵĿǰδʵ�֣�

	//int matrixType���������ͣ����������Ƿ���ϡ�����Ŀǰ�����������ݣ���ʱע��δʵ�֡�
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

	//ͼ�����ͣ���RGB��BGR��YUV֮�ࣨ����ЩֵĿǰδʵ�֣�
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