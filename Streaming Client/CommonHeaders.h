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

/*---------------------自定义常量区，少的话这里写，多的话，不如用map，例如rtsp错误信息--------------------*/
//服务器自定义结构体中涉及的服务器信息
enum ServerInfoDefine
{
	srvRsv = 0,       //默认值，保留！
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

//配置文件
const string srvSettingFile = "config/ServerInfo.xml";             //服务器信息配置文件
const string rtspErrFile = "config/static/rtspErrCodeList.csv";    //rtsp错误消息

//全局事件
