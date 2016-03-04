/*--Author：李宏杰--*/

#include "middleWare.h"

//加载播放器模块
#include "cvPlayer.h"

//加载控制信令处理模块
#include "msgHandler.h"

//加载网络处理模块
#include "cnctHandler.h"

//加载媒体缓存
#include "imageQueue.h"

mwPlayCtrl* mwPlayCtrl::instance = new mwPlayCtrl;

mwPlayCtrl * mwPlayCtrl::getInstance()
{
	return instance;
}

void mwPlayCtrl::startMiddleWare()
{
	CreateThread(NULL, NULL, mwCtrlMsgThread, NULL, NULL, NULL);

	CreateThread(NULL, NULL, mwMediaThread, NULL, NULL, NULL);
}

DWORD mwPlayCtrl::mwCtrlMsgThread(LPVOID lparam)
{
	cvPlayer *playerModule = cvPlayer::getInstance();

	msgHandler *ctrlMsgModule = msgHandler::getInstance();

	cnctHandler *networkModule = cnctHandler::getInstance();

	while (1)
	{
		//1.取走控制按键
		WaitForSingleObject(hsNewCtrlKey, INFINITE);

		char key = 0;
		key = playerModule->getCtrlKey();

		//2.把控制按键交给控制信令处理模块编码之，并取回编码结果
		string msg;
		msg = ctrlMsgModule->keyboardMsgEncode(key);

		//3.把编码好的信令交由网络模块发送之
		int bytesSent = 0;
		bytesSent = networkModule->sendMessage(msg);
	}

	return 0;
}

DWORD mwPlayCtrl::mwMediaThread(LPVOID lparam)
{
	imgBuffer *imgBuf = imgBuffer::getInstance();

	while (1)
	{
		//TODO 1.等待RTP模块接收到数据并解包

		//TODO 2.把数据送入图片缓存（信号量在缓存中激活）
	}

	return 0;
}

mwPlayCtrl::mwPlayCtrl()
{

}