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
int ctrlMsgHandler::decodeMsg(string msg)
{
	int msgType;

	/*
		先区分是键盘信令还是鼠标信令，并提取出会话号
	*/

	int session;

	allMsgHead *publicHead = (allMsgHead *)&msg;

	msgType = publicHead->payloadType;

	session = publicHead->session;

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

	return session;
}

string ctrlMsgHandler::encodeMsg(imgHead head, int imgSize, int session)
{
	string msg;

	imgMsgHead msgHead;

	//填写结构体
	msgHead.imgChannels = head.channels;
	msgHead.imgCols = head.cols;
	msgHead.imgRows = head.rows;
	msgHead.imgType = head.imgType;

	msgHead.payloadType = OPENCV_MAT;

	msgHead.msgType = IMG_HEAD;

	msgHead.msgFlag = 0x03;

	msgHead.msgSize = imgSize + 96 / sizeof(BYTE);

	//转成字符串
	msg = to_string(msgHead.msgSize) + to_string(msgHead.msgType) + to_string(msgHead.msgFlag) + to_string(msgHead.imgRows) + to_string(msgHead.imgCols) + to_string(msgHead.imgChannels) + to_string(msgHead.imgType) + to_string(msgHead.payloadType);

	//加上公共头
	msg = encodePublicHead(IMG_MSG, session, msg.size()) + msg;

	return msg;
}

string ctrlMsgHandler::encodePublicHead(int payloadType, int session, int size)
{
	string msg;

	allMsgHead publicHead;

	//填写结构体
	publicHead.cks = 0;
	publicHead.payloadType = payloadType;
	publicHead.session = session;
	publicHead.msgSize = 64 / sizeof(BYTE) + size;

	//转成字符串
	msg = to_string(publicHead.msgSize) + to_string(publicHead.payloadType) + to_string(publicHead.session) + to_string(publicHead.msgSize);

	return msg;
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