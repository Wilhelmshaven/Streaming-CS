/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	播放器到控制信令处理器的中间件

	使用：

	startMiddleWare()：启动中间件
*/
class middleWare
{
public:
	
	static middleWare* getInstance();

	void startMiddleWare();

	void shutdownAll();

private:

	void initHandles();

	/*
		线程，控制流与数据流什么的
		注意，某些信号量可以设置一下上限，太多有可能堵着
	*/

	static DWORD WINAPI mw_Player_Ctrl_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Ctrl_Net_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTSP_Net_RTSP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Net_RTP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTP_Buf_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Buf_Player_Thread(LPVOID lparam);

	//专门发送心跳的
	//static DWORD WINAPI mw_HeartBeat(LPVOID lparam);

	static void sendRTSPMsg(int method);

	static void recvRTSPMsg(int &errCode);

	/*
		单例模式相关
	*/

	middleWare();

	static middleWare *instance;

	middleWare(const middleWare &);
	middleWare &operator=(const middleWare &);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo Garbo;
};