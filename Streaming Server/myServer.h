#pragma once
#include "CommonHeaders.h"

#define myNS myHandlerName

/*
	信号量/互斥量/事件
*/
namespace myHandlerName
{
	//全局事件：结束服务器的事件
	HANDLE heShutdownSrv;

	//图像源出入口
	HANDLE hsRenderInput;
	HANDLE hsRenderOutput;

	//缓存出入口
	HANDLE hsImgBufferInput;
	HANDLE hsImgBufferOutput;
}

//class myMessage
//{
//public:
//	myMessage();
//};
//
