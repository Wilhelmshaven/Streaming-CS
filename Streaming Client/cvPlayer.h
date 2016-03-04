/*--Author：李宏杰--*/

/*
	基于OpenCV的播放器（模块）

	使用OpenCV的原因：比较熟悉，编写快捷，也能够轻易获取键盘输入，且该部分本身不属于核心内容

	设计：一个播放器，从队列中取出图像并播放，同时获取键盘消息
*/

#pragma once
#include "CommonHeaders.h"

//载入图像缓存模块
#include "imageQueue.h"

#include "opencv2\highgui\highgui.hpp"

using namespace cv;

/*
	基于OpenCV的播放器，单例模式

	使用：

	void play()：启动播放器

	static void setFrameRate(int frameRate = 20)：指定帧率，单位为帧数每秒

	void shutdown()：关闭播放器（但仍可以重启）

	void destroyPlayer()：摧毁播放器，顾名思义，调用以后你再也不能重启播放器了，只能重启客户端

	char getCtrlKey()：返回用户输入的按键
*/
class cvPlayer
{
public:

	static cvPlayer* getInstance();

	void play();

	static void setFrameRate(int frameRate = 20);

	void shutdown();

	void destroyPlayer();

	static char getCtrlKey();

private:

	//控制指令队列
	static queue<char> cmdQueue;

	//帧率（这里的意义是两帧之间间隔的毫秒数，即帧率的倒数）
	static int frameRate;

	/*
		线程相关：播放器肯定是要线程的嘛……
	*/

	//开始事件
	static HANDLE hEventStart;

	//结束事件（但仍可再开始）
	static HANDLE hEventShutdown;

	//彻底结束事件（导致退出线程）
	static HANDLE hEventDestroy;

	//播放器线程
	static DWORD WINAPI playThreadFunc(LPVOID lparam);

	/*
		单例模式相关
	*/

	cvPlayer();

	static cvPlayer *instance;

	cvPlayer(const cvPlayer &);
	cvPlayer &operator=(const cvPlayer &);

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

//播放器模块：获取到了操作，通知中间件取走
static HANDLE hsNewCtrlKey = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);