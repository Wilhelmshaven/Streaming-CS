/*--Author：李宏杰--*/

#include "msgHandler.h"

//加载中间件模块
#include "middleWare.h"

msgHandler* msgHandler::instance = new msgHandler;

msgHandler * msgHandler::getInstance()
{
	return instance;
}

string msgHandler::ctrlMsgEncode()
{
	string msg;

	//联合体，主要是便于编码，对信令结构一目了然的同时也能轻松转为一串
	union
	{
		char msg[8];

		struct
		{
			BYTE msgType;
			BYTE is_pressed;
			WORD virtualCode;
			DWORD unicode;
		}msgHead;
	}unionMsg;

	mwPlayCtrl* middleWare = mwPlayCtrl::getInstance();

	//从中间件中获取Unicode，BUT不保证队列中有按键
	unionMsg.msgHead.unicode = middleWare->getCtrlKey();

	//转Unicode为虚拟码
	unionMsg.msgHead.virtualCode = VkKeyScan(unionMsg.msgHead.unicode);

	unionMsg.msgHead.is_pressed = KB_CLICK;
	unionMsg.msgHead.msgType = KB_ENG;

	msg = unionMsg.msg;

	//调整信号量，示意下一个模块可调用
	ReleaseSemaphore(hsMsgHandler, 1, NULL);

	//Todo：主机字节序转为网络字节序
	//其实如果两边都不转……也没啥事……

	return msg;
}

msgHandler::msgHandler()
{

}