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

//加载RTSP信令处理模块
#include "rtspHandler.h"

mwPlayCtrl* mwPlayCtrl::instance = new mwPlayCtrl;

mwPlayCtrl * mwPlayCtrl::getInstance()
{
	return instance;
}

void mwPlayCtrl::startMiddleWare()
{
	CreateThread(NULL, NULL, mwCtrlMsgThread, NULL, NULL, NULL);

	CreateThread(NULL, NULL, mwMediaThread, NULL, NULL, NULL);

	CreateThread(NULL, NULL, mwHeartBeat, NULL, NULL, NULL);

	/*
		设置一下流媒体
	*/

	cnctHandler *networkModule = cnctHandler::getInstance();

	rtspHandler *rtspModule = rtspHandler::getInstance();

	rtspModule->setHandler(networkModule->getDisplayAddr());
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

	cnctHandler *networkModule = cnctHandler::getInstance();

	rtspHandler *rtspModule = rtspHandler::getInstance();

	image img;

	while (1)
	{
		/*
			1. 若网络模块中有数据，则取出
		*/
		string incomingMsg;

		WaitForSingleObject(hsNewRecvMsg, INFINITE);

		networkModule->getRecvMessage(incomingMsg);

		/*
			2. 区分接收到的数据是信令还是媒体
		*/

		if (incomingMsg[0] == '$')
		{
			//媒体数据

			//3.使用RTP模块解包
			rtpModule->unpackRTP(incomingMsg);

			//4. 取出解包好的数据
			WaitForSingleObject(hsRTPUnpacked, INFINITE);

			rtpModule->getMedia(img.vec);

			//5.把解包好的数据送入图片缓存（信号量在缓存中激活）！并停止计时
			imgBuf->pushBuffer(img.head, img.vec);

			timingModule->endTiming();
		}
		else
		{
			//信令数据
			cout << "Receive Message:" << incomingMsg << endl;

			//3.区分是什么类型的信令，看信令头。客户端的区分只需要区分RTSP信令和图像头信令
			if (incomingMsg.find("RTSP"))
			{
				//RTSP信令

				//4.调用RTSP模块解码
				int errCode = 200;

				errCode = rtspModule->decodeMsg(incomingMsg);

				//如果OK才能继续发，对吧
				if (errCode == 200)
				{
					ReleaseSemaphore(hsIsRTSPOK, 1, NULL);
				}
			}
			else
			{
				//图像头信令

				//4.取出图像头数据
				imgMsgCombine *imgMsg = (imgMsgCombine *)&incomingMsg;

				img.head.imgType = imgMsg->imgHead.imgType;

				img.head.channels = imgMsg->imgHead.imgChannels;

				img.head.xAxis.cols = imgMsg->imgHead.imgCols;

				img.head.yAxis.rows = imgMsg->imgHead.imgRows;
			}
		}		
	}

	return 0;
}

DWORD mwPlayCtrl::mwHeartBeat(LPVOID lparam)
{
	cnctHandler *networkModule = cnctHandler::getInstance();

	rtspHandler *rtspModule = rtspHandler::getInstance();

	string msg;

	WaitForSingleObject(heStartHeartBeat, INFINITE);

	while (1)
	{
		if (WaitForSingleObject(heStartHeartBeat, 0) != WAIT_OBJECT_0)
		{
			break;
		}

		/*
			每30秒发送一次心跳
		*/

		Sleep(30000);

		msg = rtspModule->encodeMsg(GET_PARAMETER);

		networkModule->sendMessage(msg);
	}

	return 0;
}

mwPlayCtrl::mwPlayCtrl()
{

}