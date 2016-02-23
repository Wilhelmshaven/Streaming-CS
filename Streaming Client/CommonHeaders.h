#pragma once

//I/O Headers
#include <iostream>
#include <fstream>

// STL Headers
#include <string>
#include <map>
#include <queue>

// Winsock Headers
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//
#include <stdlib.h>
#include <Windows.h>

using namespace std;

/*---------------------�Զ��峣�������ٵĻ�����д����Ļ���������map������rtsp������Ϣ--------------------*/
//�������Զ���ṹ�����漰�ķ�������Ϣ
enum ServerInfoDefine
{
	srvRsv = 0,       //Ĭ��ֵ��������
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

//ȫ���¼�
