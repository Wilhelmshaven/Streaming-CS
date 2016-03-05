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

//加载RTP数据包处理模块
#include "rtpHandler.h"

//加载监控模块
#include "monitor.h"

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

	monitor *timingModule = monitor::getInstance();

	while (1)
	{
		//1.取走控制按键！并开始计时
		WaitForSingleObject(hsNewCtrlKey, INFINITE);

		timingModule->beginTiming();

		char key = 0;
		key = playerModule->getCtrlKey();

		//2.把控制按键交给控制信令处理模块编码之，并取回编码结果
		string msg;
		msg = ctrlMsgModule->keyboardMsgEncode(key);

		//3.把编码好的信令交由网络模块发送之
		networkModule->sendMessage(msg);
	}

	return 0;
}

DWORD mwPlayCtrl::mwMediaThread(LPVOID lparam)
{
	imgBuffer *imgBuf = imgBuffer::getInstance();

	rtpHandler *rtpModule = rtpHandler::getInstance();

	monitor *timingModule = monitor::getInstance();

	while (1)
	{
		image img;

		//TODO 1.等待网络模块（主函数）接收到RTP包，取回包和图像头
		string packet;
		
		//TODO 2.使用RTP模块解包
		rtpModule->unpackRTP(packet);

		//3. 取出解包好的数据
		WaitForSingleObject(hsRTPUnpacked, INFINITE);

		rtpModule->getMedia(img.vec);

		//4.把解包好的数据送入图片缓存（信号量在缓存中激活）！并停止计时
		imgBuf->pushBuffer(img.head, img.vec);

		timingModule->endTiming();
	}

	return 0;
}

mwPlayCtrl::mwPlayCtrl()
{

}