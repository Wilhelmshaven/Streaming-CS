/*--Author：李宏杰--*/

#include "crtlMsgHandler.h"

//加载中间件
#include "middleWare.h"

ctrlMsgHandler* ctrlMsgHandler::instance = new ctrlMsgHandler;

ctrlMsgHandler * ctrlMsgHandler::getInstance()
{
	return instance;
}

/*
	解码
*/
void ctrlMsgHandler::decodeMsg(int msgType, string msg)
{
	switch (msgType)
	{
	case KB_MSG:
	{
		//将传入信令装入键盘信令结构体
		keyboardMsg *kbMsg = (keyboardMsg *)&msg;

		//转换虚拟码为字符
		char key;
		key = MapVirtualKey(kbMsg->unicode, MAPVK_VK_TO_CHAR);

		//TODO：发送字符给中间件或是渲染器




		break;
	}
	case MOUSE_MSG:
		break;
	default:
		break;
	}
}
