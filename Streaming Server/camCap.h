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
using namespace cv;

/*
	摄像头处理类

	使用方法：
	void camCap::startCapture()：开启摄像头；
	void camCap::stopCapture()：关闭摄像头；

	void camCap::changeFrameRate(double frameRate)：改变帧率，单位为每秒帧数

	int camCap::getHeight()：得到当前帧的高度（行数）
	int camCap::getWidth()：得到当前帧的宽度（列数）
	int camCap::getChannels()：获取帧的通道数
	int camCap::getType()：获取帧的类型（即矩阵类型）（参见cvdef.h）

	void camCap::writeBuf(vector<int> *vec)：写缓存，传入vector<int>类型的缓存以供写入

	void camCap::showImg()：开启/关闭视频窗口，若开则关，若关则开。初始状态为开启。
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

	//获取帧的高度（行）
	int getHeight();

	//获取帧的宽度（列）
	int getWidth();

	//获取帧的通道数
	int getChannels();

	//获取帧的类型（即矩阵类型）（参见cvdef.h）
	int getType();

	//取出图像写入缓存
	void writeBuf(vector<int> *vec);

	//是否需要观看摄像头图像？如果未显示，调用则显示，否则取消显示
	void showImg();

	~camCap();

private:
	//基础矩阵结构（帧）
	static Mat cvFrame;

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