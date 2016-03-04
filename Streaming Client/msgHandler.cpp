/*--Author：李宏杰--*/

#include "msgHandler.h"

msgHandler* msgHandler::instance = new msgHandler;

msgHandler * msgHandler::getInstance()
{
	return instance;
}

string msgHandler::keyboardMsgEncode(char key)
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

	unionMsg.msgHead.unicode = key;

	//转Unicode为虚拟码
	unionMsg.msgHead.virtualCode = VkKeyScan(unionMsg.msgHead.unicode);

	unionMsg.msgHead.is_pressed = KB_CLICK;
	unionMsg.msgHead.msgType = KB_ENG;

	msg = unionMsg.msg;

	//Todo：主机字节序转为网络字节序
	//其实如果两边都不转……也没啥事……

	return msg;
}

string msgHandler::mouseMsgEncode()
{
	string msg;

	//TODO：鼠标信令编码实现

	return msg;
}

msgHandler::msgHandler()
{

}