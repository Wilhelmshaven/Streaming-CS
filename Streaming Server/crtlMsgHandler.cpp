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
		char key = MapVirtualKey(kbMsg->unicode, MAPVK_VK_TO_CHAR);

		/*
			发送字符给中间件转给渲染器
		*/

		KBQueue.push(key);

		ReleaseSemaphore(hsKBCtrlMsg, 1, NULL);

		break;
	}
	case MOUSE_MSG:
	{
		//TODO：反正我现在不写……

		break;
	}
	default:
		break;
	}
}

char ctrlMsgHandler::getKBCtrlKey()
{
	char key = 0;

	if (!KBQueue.empty())
	{
		key = KBQueue.front();

		KBQueue.pop();
	}

	return key;
}

char ctrlMsgHandler::getMouseCtrlKey()
{
	//TODO：反正我现在不写……

	return 0;
}
