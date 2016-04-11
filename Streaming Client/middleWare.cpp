/*--Author������--*/

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
		�����ø����߳�
	*/

	CreateThread(NULL, NULL, mw_Player_Ctrl_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Ctrl_Net_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_RTSP_Net_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Net_RTSP_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Net_RTP_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_RTP_Buf_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Buf_Player_Thread, NULL, NULL, NULL);

	/*
		������Щ��Ҫ�ֶ�������ģ�����������
	*/

	rtsp->setHandler(network->getDisplayAddr());

	network->connectServer();
}

DWORD mwPlayCtrl::mw_Player_Ctrl_Thread(LPVOID lparam)
{
	return 0;
}



/*
	�����߳�
	��������TCP����Ļ����Ͳ�����
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
//			ÿ30�뷢��һ������
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