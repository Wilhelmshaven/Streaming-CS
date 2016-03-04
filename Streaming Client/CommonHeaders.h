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

//配置文件
const string srvSettingFile = "config/ServerInfo.xml";             //服务器信息配置文件
const string rtspErrFile = "config/static/rtspErrCodeList.csv";    //rtsp错误消息

/*
	全局信号量，为模块之间的信息流服务
	各模块是没办法知道前一个模块什么时候有信息过来的
	所以信号量就是必需品了

	TODO：以后可以改成专门的Ctrl Center来管理
*/

//播放器模块：获取到了操作，通知中间件取走
extern HANDLE hsNewCtrlKey;

//媒体缓存模块：标记有图像了，请取走播放
extern HANDLE hsPlayBuffer;

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