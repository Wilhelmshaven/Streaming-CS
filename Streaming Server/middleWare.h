/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	消息中间件
	
	使用：

	请务必先调用startMiddleWare()启动本模块
	关闭依赖于服务器关闭事件
*/

class mwMsg
{
public:

	static mwMsg* getInstance();

	void startMiddleWare();

private:

	static DWORD WINAPI mwCtrlMsgThread(LPVOID lparam);

	static DWORD WINAPI mwRTSPMsgThread(LPVOID lparam);

	/*
		单例模式
	*/

	mwMsg();

	static mwMsg* instance;
	
	mwMsg(const mwMsg&);
	mwMsg &operator=(const mwMsg&);
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