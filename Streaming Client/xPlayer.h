#pragma once

#include "CommonHeaders.h"

#include <thread>

typedef struct myImage
{
	imgHead head;

	shared_ptr<vector<BYTE>> imgData;
};

class xPlayer
{
public:

	static xPlayer* getInstance();

	//初始化函数：有就写，写构造也不是不可以
	void startPlayer();

	//入口：推入图像文件
	static void insertImage(imgHead head, shared_ptr<vector<BYTE>> image);

	//出口，弹出用户输入的按键
	static bool getCtrlKey(unsigned char &key);

private:

	//控制指令队列
	static queue<unsigned char> cmdQueue;

	//帧队列
	static queue<myImage> imgQueue;

	//播放器线程
	static DWORD WINAPI playThreadFunc(LPVOID lparam);

	//从队列里获取帧
	static bool getImage(myImage &img);

	/*
		单例模式相关
	*/

	xPlayer();
	virtual ~xPlayer();

	static xPlayer *instance;

	xPlayer(const xPlayer &);
	xPlayer &operator=(const xPlayer &);

	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo Garbo;

	// xx
	//

	std::thread playerThread;
};