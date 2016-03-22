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

	void camCap::changeFrameRate(double frameRate)：改变帧率，单位为每秒帧数

	void camCap::showImg()：开启/关闭视频窗口，若开则关，若关则开。初始状态为开启。

	（入口方法）static void render(char cmd = 0)：输入指令渲染图像
	（入口信号）static HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(renderImage))

	（出口方法）Mat getImage()：获取图像
	（出口信号）static HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(renderDone))
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
	void changeFrameRate(double frameRate);

	//是否需要观看摄像头图像？如果未显示，调用则显示，否则取消显示
	void showImg();

	//输入指令渲染图像
	static void render(char cmd = 0);

	//获取图像
	Mat getImage();

	~camCap();

private:

	//图像队列
	static queue<Mat> imgQueue;

	//控制指令队列，供中间件写入
	static queue<char> cmdQueue;

	//开始抓取事件
	static HANDLE hEventStartCap;

	//结束线程事件	
	static HANDLE hEventShutDown;

	//展示帧事件
	static HANDLE hEventShowImg;

	//线程参数结构体
	typedef struct captureThreadParam
	{
		//等待时间（帧率的倒数，单位毫秒）
		int capRate;
	};
	captureThreadParam *capParam;

	/*
		抓取摄像头的处理线程
		参数：通过captureThreadParam结构体传入
	*/
	static DWORD WINAPI captureThread(LPVOID lparam);
	
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