#include "xPlayer.h"

#include "logger.h"

#include "monitor.h"

namespace xpNS
{
	monitor *playerClock = monitor::getInstance();
	logger *playerLogger = logger::getInstance();

	//播放器模块：图像被输入
	HANDLE hsPlayerInput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerInput);

	//播放器模块：获取到了操作，通知中间件取走
	HANDLE hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

	HANDLE heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::ESCPressed);

	//超时
	//HANDLE hsTimeOut = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::timeOut);
}
using namespace xpNS;

queue<unsigned char> xPlayer::cmdQueue;
queue<myImage> xPlayer::imgQueue;

xPlayer* xPlayer::instance = new xPlayer;

xPlayer * xPlayer::getInstance()
{
	return instance;
}

void xPlayer::startPlayer()
{
	//TODO：添加你的初始化代码，如果需要

}

void xPlayer::insertImage(imgHead head, shared_ptr<vector<BYTE>> image)
{
	myImage img;
	img.head = head;
	img.imgData = image;

	imgQueue.push(img);

	ReleaseSemaphore(hsPlayerInput, 1, NULL);
}

bool xPlayer::getCtrlKey(unsigned char & key)
{
	if (cmdQueue.empty())
	{
		return false;
	}

	key = cmdQueue.front();

	cmdQueue.pop();

	return true;
}

DWORD xPlayer::playThreadFunc(LPVOID lparam)
{
	imgHead imageHead;

	myImage img;

	BYTE inputKey;

	shared_ptr<vector<BYTE>> imageData;

	while (1)
	{
		if (WaitForSingleObject(hsPlayerInput, 0) == WAIT_OBJECT_0)
		{
			if (getImage(img))
			{
				//取出图像数据
				imageData = img.imgData;

				//取出图像头
				imageHead = img.head;

				/*
					TODO：添加播放代码
					图像数据在shared_ptr<vector<BYTE>> imageData里
					图像头在imgHead imageHead里
					给你原样传过来了，可以还原图像了吧
				*/







			}
		}

		if (1)          // TODO：改为if(有按键输入）
		{
			//有输入，则通知中间件来取走
			cmdQueue.push(inputKey);

			ReleaseSemaphore(hsPlayerOutput, 1, NULL);
		}
	}

	/*
	//当然上面的while可以整段替换为下面的，因为上面在反复检查

	while (1)
	{
		WaitForSingleObject(hsPlayerInput, INFINITE);
		
		if (getImage(img))
		{
			//取出图像数据
			imageData = img.imgData;

			//取出图像头
			imageHead = img.head;

			/*
				TODO：添加播放代码
				图像数据在shared_ptr<vector<BYTE>> imageData里
				图像头在imgHead imageHead里
				给你原样传过来了，可以还原图像了吧
			*//*





		}

		//然后用另外一个线程监听输入就好了

		while (1)
		{
			if (1)          // TODO：改为if(有按键输入）
			{
				//有输入，则通知中间件来取走
				cmdQueue.push(inputKey);

				ReleaseSemaphore(hsPlayerOutput, 1, NULL);
			}
		}
	}
	*/

	return 0;
}

bool xPlayer::getImage(myImage & img)
{
	if (imgQueue.empty())
	{
		return false;
	}

	img = imgQueue.front();

	imgQueue.pop();

	return true;
}

xPlayer::xPlayer()
{

}