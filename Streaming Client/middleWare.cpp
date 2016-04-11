/*--Author：李宏杰--*/

#include "middleWare.h"

#include "cvPlayer.h"
#include "ctrlMsgHandler.h"
#include "imageBuffer.h"
#include "rtpHandler.h"
#include "rtspHandler.h"

#include "cnctHandler.h"

mwPlayCtrl* mwPlayCtrl::instance = new mwPlayCtrl;

mwPlayCtrl * mwPlayCtrl::getInstance()
{
	return instance;
}

void mwPlayCtrl::startMiddleWare()
{
	cnctHandler *network = cnctHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

	/*
		创建好各子线程
	*/

	CreateThread(NULL, NULL, mw_Player_Ctrl_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Ctrl_Net_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_RTSP_Net_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Net_RTSP_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Net_RTP_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_RTP_Buf_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Buf_Player_Thread, NULL, NULL, NULL);

	/*
		调用那些需要手动启动的模块的启动函数
	*/

	rtsp->setHandler(network->getDisplayAddr());

	network->connectServer();
}

DWORD mwPlayCtrl::mw_Player_Ctrl_Thread(LPVOID lparam)
{
	return 0;
}



/*
	心跳线程
	但现在是TCP传输的话，就不用了
*/
//DWORD mwPlayCtrl::mwHeartBeat(LPVOID lparam)
//{
//	cnctHandler *networkModule = cnctHandler::getInstance();
//
//	rtspHandler *rtspModule = rtspHandler::getInstance();
//
//	string msg;
//
//	WaitForSingleObject(heStartHeartBeat, INFINITE);
//
//	while (1)
//	{
//		if (WaitForSingleObject(heStartHeartBeat, 0) != WAIT_OBJECT_0)
//		{
//			break;
//		}
//
//		/*
//			每30秒发送一次心跳
//		*/
//
//		Sleep(30000);
//
//		msg = rtspModule->encodeMsg(GET_PARAMETER);
//
//		networkModule->sendMessage(msg);
//	}
//
//	return 0;
//}

mwPlayCtrl::mwPlayCtrl()
{

}