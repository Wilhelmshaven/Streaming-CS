/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	消息中间件（单例）
	
	使用：

	请务必先调用startMiddleWare()启动本模块
	关闭依赖于服务器关闭事件
*/
class middleWare
{
public:

	static middleWare* getInstance();

	void startMiddleWare();

private:

	void initHandles();

	static DWORD WINAPI mw_Cam_Buf_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Buf_RTP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTP_Cnct_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Cnct_RTSP_Cnct_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Cnct_Ctrl_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Ctrl_Cam_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Cnct_Web_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Web_Cnct_Thread(LPVOID lparam);

	/*
		单例模式
	*/

	middleWare();

	static middleWare* instance;
	
	middleWare(const middleWare&);
	middleWare &operator=(const middleWare&);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo garbo;

};