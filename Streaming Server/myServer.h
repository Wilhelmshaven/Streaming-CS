#pragma once
#include "CommonHeaders.h"

#define myNS myHandlerName

/*
	�ź���/������/�¼�
*/
namespace myHandlerName
{
	//ȫ���¼����������������¼�
	HANDLE heShutdownSrv;

	//ͼ��Դ�����
	HANDLE hsRenderInput;
	HANDLE hsRenderOutput;

	//��������
	HANDLE hsImgBufferInput;
	HANDLE hsImgBufferOutput;

	//��ý������������
	HANDLE hsRTSPPlay;
	HANDLE hsRTSPPause;
	HANDLE hsRTSPStop;

	//
	HANDLE hsCtrlMsgDecoded;
	HANDLE hsCtrlMsgEncoded;

	HANDLE hsRTPEncoded;

	HANDLE hsMsgArrivedRTSP;
	HANDLE hsMsgArrivedCtrl;
}

class myMessage
{
public:

	myMessage();

};