/*--Author：李宏杰--*/

/*
	摄像头抓取模块：camCap.cpp, camCap.h

	作用：
		抓取摄像头图像并转换为系统要求的格式，并在用户调用的时候返回图像（以一行一维数组的形式）。

	说明：
		独立的一个模块，这个本身和系统没有关系，所以在公共头里面不包括OpenCV的东西（在这里包含）。
*/
#pragma once
#include "CommonHeaders.h"

#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
using namespace cv;

typedef struct myImage
{
	SOCKET index;

	imgHead head;

	vector<BYTE> frame;
};

typedef struct myCommand
{
	SOCKET index;
	unsigned char key;
};

/*
	摄像头处理类

	使用方法：
	1.调用startCapture()启动；
	2.调用render()传入命令；
	3.调用getImage()获取图像；
	4.调用stopCapture()结束；

	接口：

	void camCap::startCapture()：开启摄像头；
	void camCap::stopCapture()：关闭摄像头；

	void camCap::changeFrameRate(unsigned int frameRate)：改变帧率，单位为每秒帧数

	void camCap::showImg()：开启/关闭视频窗口，若开则关，若关则开。初始状态为开启。

	void render(SOCKET index, unsigned char cmd = 0)：输入指令渲染图像

	bool getImage(SOCKET &index, Mat frame)：获取图像

	int getFrameRate()：获取帧率
*/
class camCap
{
public:
	//摄像头处理类单例
	static camCap *getInstance();

	//开启摄像头
	void startCapture();

	//关闭摄像头
	void stopCapture();

	//改变帧率
	static void changeFrameRate(unsigned int frameRate);

	//是否需要观看摄像头图像？如果未显示，调用则显示，否则取消显示
	void showImg();

	//输入指令渲染图像
	static void render(SOCKET index, unsigned char cmd = 0);

	//获取图像
	bool getImage(SOCKET &index, imgHead &head, vector<BYTE> &frame);

	//获取帧率
	static int getFrameRate();

	~camCap();

private:

	//图像队列
	static queue<myImage> imgQueue;

	//控制指令队列，供中间件写入
	static queue<myCommand> cmdQueue;

	//开始抓取事件
	static HANDLE hEventStartCap;

	//结束线程事件	
	static HANDLE hEventShutDown;

	//展示帧事件
	static HANDLE hEventShowImg;

	//抓取摄像头的处理线程
	static DWORD WINAPI captureThread(LPVOID lparam);

	//帧率，单位毫秒
	static unsigned int capRate;

	//帧率
	static unsigned int frameRate;
	
	/*
		单例模式相关
	*/
	static camCap *instance;
	camCap();

	camCap(const camCap &);
	camCap &operator=(const camCap &);

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