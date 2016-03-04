/*--Author������--*/

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

//���ڽ�����ת��ΪWindows������
#include <WinUser.h>

using namespace std;

/*---------------------�Զ��峣�������ٵĻ�����д����Ļ���������map������rtsp������Ϣ--------------------*/
//�������Զ���ṹ�����漰�ķ�������Ϣ����ȫ�ֿ���
//�ࣺcnctHandler
enum ServerInfoDefine
{
	srvRsv = 0,        //Ĭ��ֵ��������
	srvType = 1,       //����������
	protocol = 2,      //��ַͷ
	hostName = 3,      //����
	port = 4,          //�˿�
	displayRoute = 5,  //���ŵ�ַ
	hostAddr = 6,      //�IP��ַ
	ServerArgc = 7,    //���������󣬱�ʾ����������
};

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
};

//�����ļ�
const string srvSettingFile = "config/ServerInfo.xml";             //��������Ϣ�����ļ�
const string rtspErrFile = "config/static/rtspErrCodeList.csv";    //rtsp������Ϣ

/*
	ȫ���ź�����Ϊģ��֮�����Ϣ������
	��ģ����û�취֪��ǰһ��ģ��ʲôʱ������Ϣ������
	�����ź������Ǳ���Ʒ��

	TODO���Ժ���Ըĳ�ר�ŵ�Ctrl Center������
*/

//������ģ�飺��ȡ���˲�����֪ͨ�м��ȡ��
extern HANDLE hsNewCtrlKey;

//ý�建��ģ�飺�����ͼ���ˣ���ȡ�߲���
extern HANDLE hsPlayBuffer;

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