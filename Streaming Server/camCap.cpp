/*--Author：李宏杰--*/

#include "camCap.h"

//加入客户端管理器
#include "clientManager.h"

//日志
#include "logger.h"

//获取性能数据用，计时
//#include "monitor.h"

namespace camNS
{
	//monitor *myCamClock = monitor::getInstance();
	logger *myCamLogger = logger::getInstance();

	//摄像头：标记中间件已拿到并转发解码好的指令，请渲染器（摄像头）处理
	HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));

	//摄像头：标记图像已经渲染好，请中间件拿走
	HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

	HANDLE heStartPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::play);
	HANDLE heStopPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::stop);
	HANDLE hePausePlay = CreateEvent(NULL, TRUE, FALSE, syncManager::pause);
};

using namespace camNS;

//类内静态成员变量定义
HANDLE camCap::hEventStartCap;
HANDLE camCap::hEventShutDown;
HANDLE camCap::hEventShowImg;

queue<myImage> camCap::imgQueue;
queue<myCommand> camCap::cmdQueue;

unsigned int camCap::capRate;
unsigned int camCap::frameRate;

camCap *camCap::instance = new camCap;

/*
	私有构造函数
*/
camCap::camCap()
{
	frameRate = cameraCaptureRate;

	capRate = 1000/frameRate;

	/*
		初始化事件：手工重置模式
	*/
	hEventStartCap = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShowImg = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShutDown = CreateEvent(NULL, TRUE, FALSE, NULL);

	/*
		创建线程
	*/

	CreateThread(NULL, NULL, captureThread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, ctrlDealingThread, NULL, NULL, NULL);
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
//void camCap::changeFrameRate(unsigned int rate)
//{
//	frameRate = rate;
//
//	capRate = 1000 / frameRate;
//}

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

void camCap::render(SOCKET index, unsigned char cmd)
{
	myCommand myCmd;

	myCmd.index = index;
	myCmd.key = cmd;

	cmdQueue.push(myCmd);

	ReleaseSemaphore(hsRenderImage, 1, NULL);
}

bool camCap::getImage(SOCKET &index, imgHead &head, vector<BYTE> &frame)
{
	myImage tmp;

	if (imgQueue.empty())
	{
		return false;
	}

	tmp = imgQueue.front();
	imgQueue.pop();
	
	index = tmp.index;
	head = tmp.head;
	frame = tmp.frame;

	return true;
}

int camCap::getFrameRate()
{
	return frameRate;
}

camCap::~camCap()
{
	SetEvent(hEventShutDown);

	Sleep(50);

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
	clientManager *clientList = clientManager::getInstance();

	Mat cvFrame, subFrame;

	myImage matStruct;

	string windowTitle = "Camera Live!";

	imgHead head;

	char key = 0;

	Size s;

	//压缩参数表
	vector<int> compressParam(2);
	compressParam[0] = IMWRITE_JPEG_QUALITY;
	compressParam[1] = 100;

	//用来算帧率
	unsigned signal = 0;

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

			++signal;
			//if (signal > 86400)signal = 1;

			/*
				遍历客户端列表，以相应的参数存入相应的帧

				1.先检查是否播放，不播放不发
				2.再检查帧率，不能整除不发
				3.最后Scale
				4.压缩（统一压缩参数了，当然也可以作为客户端参数的一部分）
			*/

			auto iter = clientList->getIteratorStart();
			auto iterEnd = clientList->getIteratorEnd();

			while (iter != iterEnd)
			{
				if (!iter->second.play)
				{
					++iter;

					continue;
				}

				if (signal % (frameRate / iter->second.frameRate) != 0)
				{
					++iter;

					continue;
				}

				//if (iter->second.scaleFactor != 1)
				//{
					resize(cvFrame, subFrame, s, iter->second.scaleFactor, iter->second.scaleFactor);
				//}
				//else
				//{
				//	subFrame = cvFrame;
				//}

				/*
					以在图像上加水印的形式显示帧率以及其它相关信息
				*/

				putText(subFrame, "Frame rate: " + to_string(iter->second.frameRate), cvPoint(20, 20), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));
				putText(subFrame, "Resolution: " + to_string(subFrame.cols) + " X " + to_string(subFrame.rows), cvPoint(20, 35), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));

				/*
					填充图片头部
				*/

				head.channels = subFrame.channels();
				head.cols = subFrame.cols;
				head.rows = subFrame.rows;
				head.imgType = subFrame.type();

				/*
					填充结构体
				*/

				matStruct.index = iter->second.socket;
				matStruct.head = head;

				////不压缩的做法
				//imgData = frame.reshape(1, 1);

				//压缩为JPG
				imencode(".jpg", subFrame, matStruct.frame, compressParam);

				imgQueue.push(matStruct);

				ReleaseSemaphore(hsRenderDone, 1, NULL);

				++iter;
			}

			/*
				服务器端：若显示图像事件被设置，则显示图像
			*/

			if (WaitForSingleObject(hEventShowImg, 0) == WAIT_OBJECT_0)
			{
				//显示帧率
				putText(cvFrame, "Capture rate: " + to_string(frameRate), cvPoint(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255));

				imshow(windowTitle, cvFrame);
			}
			else
			{
				destroyWindow(windowTitle);
			}

			//等待时间间隔
			waitKey(capRate);
		}

		//若结束事件被设置，则结束线程
		if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
	}

	return 0;
}

DWORD camCap::ctrlDealingThread(LPVOID lparam)
{
	clientManager *clientList = clientManager::getInstance();

	myCommand cmdStruct;

	SOCKET index;
	char key = 0;

	double scale = 1;
	int frameRateOffset = 0;

	//等待开始信号
	WaitForSingleObject(hEventStartCap, INFINITE);

	while (1)
	{
		//若结束事件被设置，则结束线程
		if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
		if (WaitForSingleObject(hEventStartCap, 0) != WAIT_OBJECT_0)break;

		/*
			取出接收到的客户端指令并根据指令对图像做一些小变换/改变帧率
		*/

		WaitForSingleObject(hsRenderImage, INFINITE);
		
		if (!cmdQueue.empty())
		{
			cmdStruct = cmdQueue.front();
			cmdQueue.pop();

			key = cmdStruct.key;
			index = cmdStruct.index;
		}
		else
		{
			//102,Can't get decoded control msg from decodoer
			myCamLogger->logError(102);

			continue;
		}

		switch (key)
		{
			/*
				控制图像缩放倍率
			*/
		case '1':
		{
			scale = 0.5;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '2':
		{
			scale = 1;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '3':
		{
			scale = 1.5;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '4':
		{
			scale = 2;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '5':
		{
			scale = 2.5;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '6':
		{
			scale = 3;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '7':
		{
			scale = 0.2;
			clientList->changePlayFactor(index, scale);

			break;
		}

		/*
			控制帧率变化
		*/
		case ',':
		{
			frameRateOffset = -1;
			clientList->changePlayFactor(index, frameRateOffset);

			break;
		}
		case '.':
		{
			frameRateOffset = 1;
			clientList->changePlayFactor(index, frameRateOffset);

			break;
		}

		/*
			控制播放模式
		*/
		case 'p':
		{
			clientList->changePlayFactor(index, true);

			break;
		}

		default:
			break;
		}

		//若结束事件被设置，则结束线程
		if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
	}

	return 0;
}
