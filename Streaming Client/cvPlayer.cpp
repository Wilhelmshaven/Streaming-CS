/*--Author：李宏杰--*/

#include "cvPlayer.h"

//类内静态成员变量定义
HANDLE cvPlayer::heStart;
HANDLE cvPlayer::hePause;
HANDLE cvPlayer::heShutdown;
queue<char> cvPlayer::cmdQueue;
queue<Mat> cvPlayer::imgQueue;
int cvPlayer::frameRate;

//播放器模块：图像是否被输入
HANDLE hsPlayerInput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerInput);

//播放器模块：获取到了操作，通知中间件取走
HANDLE hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

cvPlayer* cvPlayer::instance = new cvPlayer;

cvPlayer * cvPlayer::getInstance()
{
	return instance;
}

/*
	构造函数
	初始化事件，创建线程
*/
cvPlayer::cvPlayer()
{
	frameRate = 50;

	heStart = CreateEvent(NULL, TRUE, FALSE, NULL);

	hePause = CreateEvent(NULL, TRUE, FALSE, NULL);

	heShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);

	CreateThread(NULL, NULL, playThreadFunc, NULL, NULL, NULL);
}

void cvPlayer::setFrameRate(int rate)
{
	frameRate = 1000 / rate;
}

void cvPlayer::play()
{
	SetEvent(heStart);

	ResetEvent(hePause);
}


void cvPlayer::pause()
{
	SetEvent(hePause);

	ResetEvent(heStart);
}

void cvPlayer::destroyPlayer()
{
	SetEvent(heShutdown);

	SetEvent(hePause);

	SetEvent(heStart);
}

//还原vector为Mat，并推入队列
void cvPlayer::insertImage(imgHead head, vector<char> image)
{
	Mat frame = Mat(image).reshape(head.channels, head.xAxis.cols);

	frame.convertTo(frame, head.imgType);

	imgQueue.push(frame);

	ReleaseSemaphore(hsPlayerInput, 1, NULL);
}

bool cvPlayer::getCtrlKey(char &key)
{
	if (cmdQueue.empty())
	{
		return false;
	}

	key = cmdQueue.front();

	cmdQueue.pop();

	return true;
}

bool cvPlayer::getImage(Mat& img)
{
	if (imgQueue.empty())
	{
		return false;
	}

	img = imgQueue.front();

	imgQueue.pop();

	return true;
}

DWORD cvPlayer::playThreadFunc(LPVOID lparam)
{
	/*
		创建一个空白图像用来显示
		首先获取屏幕大小，随后创建

		CV_[The number of bits per item][Signed or Unsigned][Type Prefix]C[The channel number]
		CV_8UC3：0-255，无符号，3通道

		建立出来的图像，每个像素都是（205,205,205）的灰色
	*/
	Mat blankImg;

	int nScreenWidth, nScreenHeight;

	nScreenWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN) / 2;

	blankImg.create(nScreenHeight, nScreenWidth, CV_8UC3);

	string windowName = "Streaming Client Media Player";
	namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	imshow(windowName, blankImg);

	Mat frame, preFrame;

	int key;

	while (1)
	{
		WaitForSingleObject(heStart, INFINITE);

		/*
			读取缓存->播放，循环
		*/

		preFrame = blankImg;

		while (1)
		{
			/*
				成功获取到图像则播放，否则检查退出事件
				重要！若没有新的一帧，则使用上一帧来播放画面
			*/
			if (WaitForSingleObject(hsPlayerInput, 0) == WAIT_OBJECT_0)
			{
				if (getImage(frame))
				{
					preFrame = frame;
				}
				else
				{
					//若没有新的一帧，则使用上一帧来播放画面
				}
			}

			imshow(windowName, preFrame);

			if (WaitForSingleObject(hePause, 0) == WAIT_OBJECT_0)
			{
				break;
			}

			//控制帧率，并等待可能的输入
			key = waitKey(frameRate);
			if (key != -1)
			{
				//有输入，则通知中间件来取走
				cmdQueue.push(key);

				ReleaseSemaphore(hsPlayerOutput, 1, NULL);
			}

		}

		if (WaitForSingleObject(heShutdown, 0) == WAIT_OBJECT_0)break;
	}
	
	return 0;
}