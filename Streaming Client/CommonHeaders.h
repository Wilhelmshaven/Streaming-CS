#pragma once

//I/O Headers
#include <iostream>
#include <fstream>

// STL Headers
#include <string>

// Winsock Headers
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//
#include <stdlib.h>

using namespace std;

//
enum ServerInfoDefine
{
	reserve = 0,       //默认值，保留！
	srvType = 1,       //服务器类型
	protocol = 2,      //地址头
	hostName = 3,      //域名
	port = 4,          //端口
	displayRoute = 5,  //播放地址
};


//RTSP信令
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

	//错误代码
};

enum CustomDefine
{
	BUF_SIZE = 8192,
	RTP_PORT = 55778
};

const string srvSettingFile = "config/ServerInfo.xml";