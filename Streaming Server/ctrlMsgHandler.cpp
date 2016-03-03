/*--Author：李宏杰--*/

#include "ctrlMsgHandler.h"

ctrlMsgHandler* ctrlMsgHandler::instance = new ctrlMsgHandler;

ctrlMsgHandler * ctrlMsgHandler::getInstance()
{
	return instance;
}

/*
	解码
*/
void ctrlMsgHandler::decodeMsg(string msg)
{
	int msgType;

	/*
		先区分是键盘信令还是鼠标信令
	*/

	allMsgHead *publicHead = (allMsgHead *)&msg;

	msgType = publicHead->payloadType;

	delete(publicHead);

	/*
		根据信令类型处理
	*/
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

		ctrlKeyQueue.push(key);

		ReleaseSemaphore(hsCtrlMsg, 1, NULL);

		delete(kbMsg);

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

char ctrlMsgHandler::getCtrlKey()
{
	char key = 0;

	if (!ctrlKeyQueue.empty())
	{
		key = ctrlKeyQueue.front();

		ctrlKeyQueue.pop();
	}

	return key;
}

ctrlMsgHandler::ctrlMsgHandler()
{

}