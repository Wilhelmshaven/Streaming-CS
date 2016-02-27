/*--Author：李宏杰--*/

#include "cvPlayer.h"

//加载中间件
#include "middleWare.h"

//类内静态成员变量定义
HANDLE cvPlayer::hEventStart;
HANDLE cvPlayer::hEventShutdown;
HANDLE cvPlayer::hEventDestroy;
int cvPlayer::frameRate;

cvPlayer* cvPlayer::instance = new cvPlayer;

cvPlayer * cvPlayer::getInstance()
{
	return instance;
}

void cvPlayer::play()
{
	SetEvent(hEventStart);
	ResetEvent(hEventShutdown);
}

void cvPlayer::setFrameRate(int rate)
{
	frameRate = 1000 / rate;
}

void cvPlayer::shutdown()
{
	SetEvent(hEventShutdown);
	ResetEvent(hEventStart);
}

void cvPlayer::destroyPlayer()
{
	SetEvent(hEventDestroy);
	SetEvent(hEventShutdown);
	SetEvent(hEventStart);
}

cvPlayer::~cvPlayer()
{

}

DWORD cvPlayer::playThreadFunc(LPVOID lparam)
{
	imgBuffer *imgBuf = imgBuffer::getInstance();

	mwPlayCtrl *midWare = mwPlayCtrl::getInstance();

	while (1)
	{
		WaitForSingleObject(hEventStart, INFINITE);

		/*
			创建一个空白图像用来显示
			首先获取屏幕大小，随后创建

			CV_[The number of bits per item][Signed or Unsigned][Type Prefix]C[The channel number]
			CV_8UC3：0-255，无符号，3通道

			建立出来的图像，每个像素都是（205,205,205）的灰色
		*/
		Mat blankImg;

		int nScreenWidth, nScreenHeight;

		nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		blankImg.create(nScreenHeight, nScreenWidth, CV_8UC3);

		string windowName = "Streaming Client Media Player";
		namedWindow(windowName, CV_WINDOW_AUTOSIZE);

		imshow(windowName, blankImg);

		/*
			读取缓存->播放，循环
		*/

		Mat frame;

		while (1)
		{
			//成功获取到图像则继续，否则检查退出事件
			if (imgBuf->popBuffer(frame))
			{
				imshow(windowName, frame);
			}

			if (WaitForSingleObject(hEventShutdown, 0) == WAIT_OBJECT_0)
			{
				break;
			}

			//控制帧率，并等待可能的输入
			int key = waitKey(frameRate);
			if (key != -1)
			{
				//有输入，转发指令，通过中间件实现
				midWare->pushCtrlKey(key);
			}
		}

		if (WaitForSingleObject(hEventDestroy, 0) == WAIT_OBJECT_0)break;
	}
	
	return 0;
}

/*
	构造函数

	主要任务：
	初始化事件，创建线程
*/
cvPlayer::cvPlayer()
{
	hEventStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventDestroy = CreateEvent(NULL, TRUE, FALSE, NULL);

	CreateThread(NULL, NULL, playThreadFunc, NULL, NULL, NULL);
}

