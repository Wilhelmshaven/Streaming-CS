/*--Author：李宏杰--*/

#include "ctrlMsgHandler.h"

namespace ctrlNS
{
	//控制信令处理模块：标记信令解码完毕，请中间件拿走转给渲染器
	HANDLE ctrlMsgDecoded = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgDecoded);

	//控制信令处理模块：标记信令编码完毕，请中间件拿走转给网络模块
	//HANDLE hsCtrlMsgEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgEncoded);
};
using namespace ctrlNS;

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
void ctrlMsgHandler::decodeMsg(SOCKET index, string msg)
{
	/*
		提取出会话号与信令类型
	*/

	auto *aHead = (allMsgHead *)msg.c_str();

	unsigned int payloadType, session;

	payloadType = aHead->payloadType;

	session = ntohl(aHead->session);

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
		unsigned char virtualkey = ntohs(kbMsg->virtualCode);
		unsigned char key = MapVirtualKey(virtualkey, MAPVK_VK_TO_CHAR);

		decodedMsg dMsg;

		dMsg.ctrlKey = tolower(key);
		dMsg.session = session;

		dMsg.index = index;

		decodedMsgQueue.push(dMsg);

		ReleaseSemaphore(ctrlMsgDecoded, 1, NULL);

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

//void ctrlMsgHandler::encodeMsg(imgHead head, unsigned int imgSize, unsigned int session)
//{
//	string msg;
//
//	msg.resize(sizeof(imgMsgHead));
//
//	auto *msgHead = (imgMsgHead *)msg.c_str();
//
//	//填写结构体
//	msgHead->imgChannels = htons(head.channels);
//	msgHead->imgCols = htons(head.cols);
//	msgHead->imgRows = htons(head.rows);
//	msgHead->imgType = head.imgType;
//
//	msgHead->payloadType = OPENCV_MAT;
//
//	msgHead->msgType = IMG_HEAD;
//
//	msgHead->msgFlag = 0x03;
//
//	msgHead->msgSize = htons(imgSize + sizeof(imgMsgHead));
//
//	//加上公共头
//	msg = encodePublicHead(IMG_MSG, session, msg.size()) + msg;
//
//	encodedMsgQueue.push(msg);
//
//	ReleaseSemaphore(hsCtrlMsgEncoded, 1, NULL);
//}

bool ctrlMsgHandler::getDecodedMsg(SOCKET &index, unsigned int &session, unsigned char &ctrlKey)
{
	if (decodedMsgQueue.empty())
	{
		return false;
	}

	decodedMsg dMsg = decodedMsgQueue.front();

	decodedMsgQueue.pop();

	session = dMsg.session;
	ctrlKey = dMsg.ctrlKey;

	index = dMsg.index;

	return true;
}

//bool ctrlMsgHandler::getEncodedMsg(string & encodedMsg)
//{
//	if (encodedMsgQueue.empty())return false;
//
//	encodedMsg = encodedMsgQueue.front();
//
//	encodedMsgQueue.pop();
//
//	return true;
//}

string ctrlMsgHandler::encodePublicHead(unsigned int payloadType, unsigned int session, unsigned int size)
{
	string msg;
	msg.resize(sizeof(allMsgHead));

	auto *publicHead = (allMsgHead *)msg.c_str();

	//填写结构体
	publicHead->cks = 0;
	publicHead->payloadType = payloadType;
	publicHead->session = htonl(session);
	publicHead->msgSize = htons(sizeof(allMsgHead) + size);

	return msg;
}