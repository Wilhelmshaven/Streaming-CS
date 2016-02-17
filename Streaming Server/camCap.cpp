/*--Author：李宏杰--*/

#include "camCap.h"

/*
	初始化摄像头处理模块的单例
*/
camCap *camCap::instance = new camCap;

/*
	私有构造函数
*/
camCap::camCap()
{
	capParam = new captureThreadParam;
	capParam->capRate = 50;

	/*
		初始化事件：手工重置模式
	*/
	hEventStartCap = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShowImg = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShutDown = CreateEvent(NULL, TRUE, FALSE, NULL);

	/*
		创建线程
	*/
	CreateThread(NULL, NULL, captureThread, capParam, NULL, NULL);
}

/*
	函数：启动摄像头
*/
void camCap::startCapture()
{
	//使用开始事件控制线程的进行
	SetEvent(hEventStartCap);

	//显示图像
	SetEvent(hEventShowImg);
}

/*
	函数：关闭摄像头
*/
void camCap::stopCapture()
{
	//使用开始事件控制线程的进行
	ResetEvent(hEventStartCap);

	//复位显示图像事件
	ResetEvent(hEventShowImg);
}

/*
	函数：改变帧率
	帧率取倒数，作为抓取的时间间隔。
*/
void camCap::changeFrameRate(double frameRate)
{
	capParam->capRate = 1000 / frameRate;
}

/*
	函数：返回帧高
*/
int camCap::getHeight()
{
	return cvFrame.rows;
}

/*
	函数：返回帧宽
*/
int camCap::getWidth()
{
	return cvFrame.cols;
}

/*
	函数：写入缓存
	描述：
		将OpenCV里Mat结构的图像写入到传入的一维数组里。

		目前不设置扩展功能，默认（指定）帧是三通道的，值为RGB。
		直接存储成一行。
*/
void camCap::writeBuf(vector<int> *vec)
{
	/*
		矩阵迭代器，直接指定是3通道模式了
		这里的迭代器的使用方法和Mat的操作方法需要参考OpenCV
	*/
	Mat thisFrame = cvFrame;

	MatIterator_<Vec3b> iter, end;
	end = thisFrame.end<Vec3b>();
	
	for (iter = thisFrame.begin<Vec3b>(); iter < end; ++iter)
	{
		//对于每一个像素，读取其RGB值存入数组
		for (int i = 0; i < 3; ++i)
		{
			vec->push_back((*iter)[i]);
		}
	}
}

/*
	函数：展示图像
	描述：通过设置事件和重置事件的方式，改变线程中的情况
*/
void camCap::showImg()
{
	if (WaitForSingleObject(hEventShowImg, 0) == WAIT_OBJECT_0)
	{
		ResetEvent(hEventShowImg);
	}
	else
	{
		SetEvent(hEventShowImg);
	}
}

camCap::~camCap()
{
	SetEvent(hEventShutDown);

	delete(capParam);

	CloseHandle(hEventStartCap);
	CloseHandle(hEventShowImg);
	CloseHandle(hEventShutDown);
}

camCap * camCap::getInstance()
{
	return instance;
}

/*
	线程处理函数

	描述：
		首先等待开始信号
		然后创建视频抓取类（就当是摄像头对象了）
		然后进入一个永久循环。
		其流程为：
			检查结束事件，若激活则退出；
			从摄像头对象中获取当前帧；
			检查是否需要显示图像；
			等待一定时间间隔；（体现帧率）
*/
DWORD WINAPI camCap::captureThread(LPVOID lparam)
{
	captureThreadParam *param = (captureThreadParam *)lparam;

	while (1)
	{
		//等待开始信号
		WaitForSingleObject(hEventStartCap, INFINITE);

		//摄像头对象
		VideoCapture capture(0);

		while (1)
		{
			//若结束事件被设置，则结束线程
			if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
			if (WaitForSingleObject(hEventStartCap, 0) != WAIT_OBJECT_0)break;

			//从设备中取出当前帧
			capture >> cvFrame;

			//若显示图像事件被设置，则显示图像
			if (WaitForSingleObject(hEventShowImg, 0) == WAIT_OBJECT_0)
			{
				imshow("Camera Live!", cvFrame);
			}
			else
			{
				destroyWindow("Camera Live!");
			}

			//等待时间间隔
			waitKey(param->capRate);
		}

		//若结束事件被设置，则结束线程
		if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
	}

	delete(param);

	return 0;
}
