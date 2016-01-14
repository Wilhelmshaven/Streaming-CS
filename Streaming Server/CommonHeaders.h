#pragma once

//I/O Headers
#include <iostream>
#include <fstream>

// STL Headers
#include <string>
#include <vector>
#include <queue>

// Winsock Headers
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//
#include <stdlib.h>

using namespace std;

//RTSP–≈¡Ó
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

	//¥ÌŒÛ¥˙¬Î
};

enum CustomDefine
{
	BUF_SIZE = 8192,
	RTP_PORT = 55778
};

#define imgVector vector<vector<vector<int>>>