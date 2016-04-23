/*--Author：李宏杰--*/

#include "camCap.h"

namespace camNS
{
	//摄像头：标记中间件已拿到并转发解码好的指令，请渲染器（摄像头）处理
	HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));

	//摄像头：标记图像已经渲染好，请中间件拿走
	HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

	HANDLE heStartPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::play);
	HANDLE heStopPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::stop);
	HANDLE hePausePlay = CreateEvent(NULL, TRUE, FALSE, syncManager::pause);

	//单客户端测试用
	HANDLE hePlay = CreateEvent(NULL, TRUE, FALSE, NULL);
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
	frameRate = 10;

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
void camCap::changeFrameRate(unsigned int rate)
{
	frameRate = rate;

	capRate = 1000 / frameRate;
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
	Mat cvFrame;

	myImage matStruct;
	myCommand cmdStruct;

	string windowTitle = "Camera Live!";

	SOCKET index;
	imgHead head;

	char key = 0;

	Size s;
	double scale = 1;

	//压缩参数表
	vector<int> compressParam(2);
	compressParam[0] = IMWRITE_JPEG_QUALITY;
	compressParam[1] = 100;

	while (1)
	{
		//等待开始信号
		WaitForSingleObject(hEventStartCap, INFINITE);

		//摄像头对象
		VideoCapture capture(0);
		int p = 0;
		while (1)
		{
			//若结束事件被设置，则结束线程
			if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
			if (WaitForSingleObject(hEventStartCap, 0) != WAIT_OBJECT_0)break;

			//从设备中取出当前帧
			capture >> cvFrame;

			/*
				取出接收到的客户端指令并根据指令对图像做一些小变换/改变帧率

				TODO：封装一下
			*/

			if (WaitForSingleObject(hsRenderImage, 0) == WAIT_OBJECT_0)
			{	
				if (!cmdQueue.empty())
				{
					cmdStruct = cmdQueue.front();
					cmdQueue.pop();

					key = cmdStruct.key;
					index = cmdStruct.index;
				}

				switch (key)
				{
				/*
					控制图像缩放倍率
				*/
				case '1':
				{
					scale = 0.5;

					break;
				}
				case '2':
				{
					scale = 1;

					break;
				}
				case '3':
				{
					scale = 1.5;

					break;
				}
				case '4':
				{
					scale = 0.2;

					break;
				}

				/*
					控制帧率变化
				*/
				case ',':
				{
					if (frameRate > 1)
					{
						//帧率至少也得有1
						--frameRate;
					}

					changeFrameRate(frameRate);

					break;
				}
				case '.':
				{
					++frameRate;
					changeFrameRate(frameRate);

					break;
				}

				/*
					控制播放模式
				*/
				case 'p':
				{
					if (WaitForSingleObject(hePlay, 0) == WAIT_OBJECT_0)
					{
						ResetEvent(hePlay);
					}
					else
					{
						SetEvent(hePlay);
					}

					break;
				}

				default:
					break;
				}
			}	

			/*
				如果处于play状态，则将变换好的图像（压缩并）塞入缓存，并释放信号量

				!!这里没有处理多客户端情况啊（原来按键才发送的做法是有的

				TODO：处理多客户端
			*/

			if ((WaitForSingleObject(hePlay, 0) == WAIT_OBJECT_0))
			{
				if (scale != 1)
				{
					resize(cvFrame, cvFrame, s, scale, scale);
				}

				////不压缩的做法
				//imgData = frame.reshape(1, 1);

				/*
					压缩为PNG
				*/
	
				imencode(".jpg", cvFrame, matStruct.frame, compressParam);

				/*
					填充图片头部
				*/

				head.channels = cvFrame.channels();
				head.cols = cvFrame.cols;
				head.rows = cvFrame.rows;
				head.imgType = cvFrame.type();

				/*
					填充结构体
				*/

				matStruct.index = index;
				matStruct.head = head;

				imgQueue.push(matStruct);

				ReleaseSemaphore(hsRenderDone, 1, NULL);
			}

			//若显示图像事件被设置，则显示图像
			if (WaitForSingleObject(hEventShowImg, 0) == WAIT_OBJECT_0)
			{
				//显示帧率
				putText(cvFrame, "Frame rate: " + to_string(frameRate), cvPoint(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255));

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
