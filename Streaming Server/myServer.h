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
}

//class myMessage
//{
//public:
//	myMessage();
//};
//
