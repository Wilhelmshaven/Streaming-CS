/*--Author：李宏杰--*/

/*
	基于OpenCV的播放器（模块）

	使用OpenCV的原因：比较熟悉，编写快捷，也能够轻易获取键盘输入，且该部分本身不属于核心内容

	设计：一个播放器，从队列中取出图像并播放，同时获取键盘消息
*/

#pragma once
#include "CommonHeaders.h"

#include "opencv2\highgui\highgui.hpp"

using namespace cv;

/*
	基于OpenCV的播放器，单例模式

	使用：

	void setFrameRate(int frameRate = 20)：指定帧率，单位为帧数每秒

	void play()：启动播放器

	void pause()：暂停播放器

	void destroyPlayer()：摧毁播放器，顾名思义，调用以后你再也不能重启播放器了，只能重启客户端

	void insertImage(imgHead head, shared_ptr<vector<BYTE>> image)：入口：推入图像文件

	bool getCtrlKey(unsigned char &key)：出口，弹出用户输入的按键

	int getFrameRate()：获取当前帧率
*/
class cvPlayer
{
public:

	static cvPlayer* getInstance();

	//指定帧率，单位为帧数每秒
	static void setFrameRate(unsigned short frameRate = 20);

	//启动播放器
	void play();

	//暂停播放器
	void pause();

	//摧毁播放器，顾名思义，调用以后你再也不能重启播放器了，只能重启客户端
	void destroyPlayer();

	//入口：推入图像文件
	static void insertImage(imgHead head, shared_ptr<vector<BYTE>> image);

	//出口，弹出用户输入的按键
	static bool getCtrlKey(unsigned char &key);

	static int getFrameRate();

private:

	//控制指令队列
	static queue<unsigned char> cmdQueue;

	//帧队列
	static queue<shared_ptr<Mat>> imgQueue;

	//帧率（这里的意义是两帧之间间隔的毫秒数，即帧率的倒数）

	static int frameRate;
	static int playRate;

	//从队列里获取帧
	static bool getImage(shared_ptr<Mat> &img);
	/*
		线程相关：播放器肯定是要线程的嘛……
	*/

	//播放器线程
	static DWORD WINAPI playThreadFunc(LPVOID lparam);

	//开始事件
	static HANDLE heStart;

	//暂停事件
	static HANDLE hePause;

	//彻底结束事件（导致退出线程）
	static HANDLE heShutdown;

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