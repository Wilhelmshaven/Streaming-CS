#pragma once

//I/O Headers
#include <iostream>
#include <fstream>

// STL Headers
#include <string>
#include <map>

// Winsock Headers
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//
#include <stdlib.h>

using namespace std;

/*---------------------�Զ��峣�������ٵĻ�����д����Ļ���������map������rtsp������Ϣ--------------------*/
//
enum ServerInfoDefine
{
	reserve = 0,       //Ĭ��ֵ��������
	srvType = 1,       //����������
	protocol = 2,      //��ַͷ
	hostName = 3,      //����
	port = 4,          //�˿�
	displayRoute = 5,  //���ŵ�ַ
	hostAddr = 6,      //�IP��ַ
	ServerArgc = 7,    //���������󣬱�ʾ����������
};


//RTSP����
enum RTSPmessage
{
	OPTION = 1,
	DESCRIBE = 2,
	SETUP = 3,
	PLAY = 4,
	GET_PARAMETER = 5,
	PAUSE = 6,
	STOP = 7,
	TEARDOWN = 8,

	//�������
};

enum CustomDefine
{
	BUF_SIZE = 8192,
	RTP_PORT = 55778
};

//�����ļ�
const string srvSettingFile = "config/ServerInfo.xml";
const string rtspErrFile = "config/static/rtspErrCodeList.csv";