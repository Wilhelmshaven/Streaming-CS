/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	播放器到控制信令处理器的中间件

	使用：

	startMiddleWare()：启动中间件
*/
class mwPlayCtrl
{
public:
	
	static mwPlayCtrl* getInstance();

	void startMiddleWare();

private:

	/*
		线程，控制流与数据流什么的
		注意，某些信号量可以设置一下上限，太多有可能堵着
	*/

	static DWORD WINAPI mw_Player_Ctrl_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Ctrl_Net_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTSP_Net_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Net_RTSP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Net_RTP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTP_Buf_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Buf_Player_Thread(LPVOID lparam);

	//专门发送心跳的
	//static DWORD WINAPI mw_HeartBeat(LPVOID lparam);

	/*
		单例模式相关
	*/

	mwPlayCtrl();

	static mwPlayCtrl *instance;

	mwPlayCtrl(const mwPlayCtrl &);
	mwPlayCtrl &operator=(const mwPlayCtrl &);
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