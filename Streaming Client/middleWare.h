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

	void pushCtrlKey(char unicode)：播放器模块向中间件输入Unicode形式的按键

	char getCtrlKey()：向信令控制模块返回Unicode形式的按键

*/
class mwPlayCtrl
{
public:
	
	static mwPlayCtrl* getInstance();

	//注意，输入为Unicode
	void pushCtrlKey(char unicode);

	char getCtrlKey();

private:

	queue<char> ctrlKeyQueue;

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