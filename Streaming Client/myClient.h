#include "CommonHeaders.h"

//用于输出公共开始信息
class myMessage
{
public:
	myMessage();
};

//这里还要仔细考虑，现在更多的是一次性程序的感觉。
//TODO：增加复位功能
HANDLE hCloseClientEvent = CreateEvent(NULL, TRUE, FALSE, NULL);      //事件：关闭客户端，初值为NULL
HANDLE hRTSPBeginEvent = CreateEvent(NULL, TRUE, FALSE, NULL);        //事件：启动RTSP收发
HANDLE hBeatStartEvent = NULL;                                        //事件：启动RTSP心跳线程，初值为NULL