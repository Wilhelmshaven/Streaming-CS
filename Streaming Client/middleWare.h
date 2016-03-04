/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	所有的中间件都写在这里
	命名：mw+两个模块单词组合

	中间件列表：

*/

/*
	播放器到控制信令处理器的中间件
	由于要保证两个模块都在使用同一个数据交换区，所以也是单例

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
	*/

	static DWORD WINAPI mwCtrlMsgThread(LPVOID lparam);

	static DWORD WINAPI mwMediaThread(LPVOID lparam);

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