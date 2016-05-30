/*--Author：李宏杰--*/

#include "cvPlayer.h"

#include "logger.h"

#include "monitor.h"

namespace cvPlayerNS
{
	monitor *playerClock = monitor::getInstance();
	logger *playerLogger = logger::getInstance();

	//播放器模块：图像是否被输入
	HANDLE hsPlayerInput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerInput);

	//播放器模块：获取到了操作，通知中间件取走
	HANDLE hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

	HANDLE heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::ESCPressed);

	//超时
	HANDLE hsTimeOut = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::timeOut);
}
using namespace cvPlayerNS;

cvPlayer* cvPlayer::instance = new cvPlayer;

//类内静态成员变量定义
HANDLE cvPlayer::heStart;
HANDLE cvPlayer::hePause;
HANDLE cvPlayer::heShutdown;

queue<unsigned char> cvPlayer::cmdQueue;
queue<myImage> cvPlayer::imgQueue;

int cvPlayer::frameRate;
int cvPlayer::playRate;

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
	playRate = CLIENT_FRAME_RATE;
	frameRate = 1000 / playRate;

	heStart = CreateEvent(NULL, TRUE, FALSE, NULL);

	hePause = CreateEvent(NULL, TRUE, FALSE, NULL);

	heShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);

	CreateThread(NULL, NULL, playThreadFunc, NULL, NULL, NULL);
}

void cvPlayer::setFrameRate(unsigned short rate)
{
	playRate = rate;

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

	SetEvent(heCloseClient);
}

void cvPlayer::insertImage(imgHead head, shared_ptr<vector<BYTE>> image)
{
	myImage img;
	img.head = head;
	img.imgData = image;

	imgQueue.push(img);

	ReleaseSemaphore(hsPlayerInput, 1, NULL);
}

bool cvPlayer::getCtrlKey(unsigned char &key)
{
	if (cmdQueue.empty())
	{
		return false;
	}

	key = cmdQueue.front();

	cmdQueue.pop();

	return true;
}

int cvPlayer::getFrameRate()
{
	return playRate;
}

bool cvPlayer::getImage(myImage &img)
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

	Mat preFrame;

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN) / 2;

	preFrame.create(nScreenHeight, nScreenWidth, CV_8UC3);

	string windowName = "Streaming Client Media Player";
	//namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	int key;

	myImage img;

	while (1)
	{
		WaitForSingleObject(heStart, INFINITE);

		/*
			读取缓存->播放，循环
		*/

		//playerClock->beginTiming();

		while (1)
		{
			/*
				成功获取到图像则播放，否则检查退出事件
				重要！若没有新的一帧，则使用上一帧来播放画面
			*/

			if (WaitForSingleObject(hsPlayerInput, 0) == WAIT_OBJECT_0)
			{
				/*
					计算时间差
				*/

				//playerClock->endTiming();

				////超时回报：直接推入小于号
				//if (WaitForSingleObject(hsTimeOut, 0) == WAIT_OBJECT_0)
				//{
				//	cmdQueue.push(',');
				//	
				//	setFrameRate(playRate - 1);

				//	ReleaseSemaphore(hsPlayerOutput, 1, NULL);
				//}

				//playerClock->beginTiming();

				if (getImage(img))
				{
					/*
					//不压缩的处理代码（还原Mat）

					////如果vector的大小不对，会爆出CV内存错误
					//Mat frame = Mat(*image).reshape(head.channels, head.yAxis.rows).clone();
					//frame.convertTo(frame, head.imgType);
					*/

					//解压缩图片
					Mat frame = imdecode(Mat(*(img.imgData)), CV_LOAD_IMAGE_COLOR);

					preFrame = frame;
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
				if (key == 27)
				{
					//ESC键被按下，退出客户端
					SetEvent(heCloseClient);
					
					break;
				}

				//控制按键频率：waitkey按下后会立即返回的，所以等待时间会不足，这里需要补偿一下
				Sleep(frameRate);

				//有输入，则通知中间件来取走
				cmdQueue.push(key);

				ReleaseSemaphore(hsPlayerOutput, 1, NULL);

				if (key == ',')
				{
					if (playRate > 0)
					{
						--playRate;
					}
#ifdef DEBUG
					cout << "Frame Rate = " << playRate << endl;
#endif
				}

				if (key == '.')
				{
					if (playRate < 20)
					{
						++playRate;
					}
#ifdef DEBUG
					cout << "Frame Rate = " << playRate << endl;
#endif
				}
			}

		}

		if (WaitForSingleObject(heCloseClient, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	
	return 0;
}