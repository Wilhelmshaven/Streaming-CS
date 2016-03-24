/*--Author：李宏杰--*/

#include "ctrlMsgHandler.h"

//控制信令处理模块：标记信令解码完毕，请中间件拿走转给渲染器
HANDLE hsCtrlMsgDecoded = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgDecoded);

//控制信令处理模块：标记信令编码完毕，请中间件拿走转给网络模块
HANDLE hsCtrlMsgEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgEncoded);

ctrlMsgHandler* ctrlMsgHandler::instance = new ctrlMsgHandler;

ctrlMsgHandler::ctrlMsgHandler()
{

}

ctrlMsgHandler * ctrlMsgHandler::getInstance()
{
	return instance;
}

/*
	解码
*/
void ctrlMsgHandler::decodeMsg(string msg)
{
	/*
		提取出会话号与信令类型
	*/

	auto *aHead = (allMsgHead *)msg.c_str();

	unsigned int payloadType, session;

	payloadType = aHead->payloadType;

	session = aHead->session;

	/*
		根据信令类型处理
	*/

	switch (payloadType)
	{
	case KB_MSG:
	{
		//将传入信令装入键盘信令结构体。注意：用string其实真心比较容易搞乱啊……
		auto kbMsg = (keyboardMsg *)(msg.c_str() + 8);

		//转换虚拟码为字符
		char key = MapVirtualKey(kbMsg->unicode, MAPVK_VK_TO_CHAR);

		decodedMsg dMsg;

		dMsg.ctrlKey = key;
		dMsg.session = session;

		decodedMsgQueue.push(dMsg);

		ReleaseSemaphore(hsCtrlMsgDecoded, 1, NULL);		

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

void ctrlMsgHandler::encodeMsg(imgHead head, int imgSize, int session)
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

	encodedMsgQueue.push(msg);

	ReleaseSemaphore(hsCtrlMsgEncoded, 1, NULL);
}

bool ctrlMsgHandler::getDecodedMsg(unsigned int &session, char &ctrlKey)
{
	if (decodedMsgQueue.empty())return false;

	decodedMsg dMsg = decodedMsgQueue.front();

	decodedMsgQueue.pop();

	session = dMsg.session;

	ctrlKey = dMsg.ctrlKey;

	return true;
}

bool ctrlMsgHandler::getEncodedMsg(string & encodedMsg)
{
	if (encodedMsgQueue.empty())return false;

	encodedMsg = encodedMsgQueue.front();

	encodedMsgQueue.pop();

	return true;
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