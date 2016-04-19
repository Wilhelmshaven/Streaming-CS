/*--Author：李宏杰--*/

#include "camCap.h"

//摄像头：标记中间件已拿到并转发解码好的指令，请渲染器（摄像头）处理
HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));

//摄像头：标记图像已经渲染好，请中间件拿走
HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

//类内静态成员变量定义
HANDLE camCap::hEventStartCap;
HANDLE camCap::hEventShutDown;
HANDLE camCap::hEventShowImg;

queue<myMat> camCap::imgQueue;
queue<myCommand> camCap::cmdQueue;

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

bool camCap::getImage(SOCKET &index, Mat &frame)
{
	myMat tmp;

	if (imgQueue.empty())
	{
		return false;
	}

	tmp = imgQueue.front();
	imgQueue.pop();
	
	index = tmp.index;
	frame = tmp.frame;

	return true;
}

camCap::~camCap()
{
	SetEvent(hEventShutDown);

	Sleep(50);

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

	Mat cvFrame;

	myMat matStruct;
	myCommand cmdStruct;

	SOCKET index;

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

			/*
				接收到客户端指令后，做相关变换，推出图像
				通过信号量控制
			*/
			if (WaitForSingleObject(hsRenderImage, 0) == WAIT_OBJECT_0)
			{	
				char key = 0;

				if (!cmdQueue.empty())
				{
					cmdStruct = cmdQueue.front();
					cmdQueue.pop();

					key = cmdStruct.key;
					index = cmdStruct.index;
				}

				/*
					根据指令对图像做一些小变换

					TODO：独立抽出作为一个/多个方法
				*/
				switch (key)
				{
				case 'a':
				{
					Size s;
					resize(cvFrame, cvFrame, s, 0.5, 0.5);

					break;
				}
				case 'd':
				{
					Size s;
					resize(cvFrame, cvFrame, s, 1.5, 1.5);

					break;
				}
				case 'p':
				{
					Size s;
					resize(cvFrame, cvFrame, s, 0.2, 0.2);

					break;
				}
				default:
					break;
				}

				/*
					将变换好的图像塞入缓存，并释放信号量
				*/

				matStruct.frame = cvFrame;
				matStruct.index = index;

				imgQueue.push(matStruct);

				ReleaseSemaphore(hsRenderDone, 1, NULL);

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
