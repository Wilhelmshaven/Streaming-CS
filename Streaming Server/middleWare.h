/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	消息中间件
	
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

	static DWORD WINAPI mwCtrlMsgThread(LPVOID lparam);

	static DWORD WINAPI mwRTSPMsgThread(LPVOID lparam);

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