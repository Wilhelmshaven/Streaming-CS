/*--Author：李宏杰--*/

#include "ctrlMsgHandler.h"

HANDLE hsCtrlMsgEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgOutput);

ctrlMsgHandler* ctrlMsgHandler::instance = new ctrlMsgHandler;

ctrlMsgHandler * ctrlMsgHandler::getInstance()
{
	return instance;
}

ctrlMsgHandler::ctrlMsgHandler()
{

}

void ctrlMsgHandler::keyboardMsgEncode(char key, unsigned long session)
{
	string msg;

	size_t size = sizeof(allMsgHead) + sizeof(keyboardMsg);

	msg.resize(size);

	auto publicHead = (allMsgHead *)msg.c_str();

	publicHead->cks = 0;
	publicHead->session = htonl(session);
	publicHead->msgSize = htons(size);
	publicHead->payloadType = KB_MSG;

	auto head = (keyboardMsg *)(msg.c_str() + 8);

	head->msgType = KB_ENG;

	head->isPressed = KB_CLICK;

	/*
		转Unicode为虚拟码
		https://msdn.microsoft.com/en-us/library/dd375731(v=vs.85).aspx
	*/
	head->virtualCode = htons(VkKeyScan(key));

	head->unicode = htonl(key);

	encodedMsgQueue.push(msg);

	ReleaseSemaphore(hsCtrlMsgEncoded, 1, NULL);
}

void ctrlMsgHandler::mouseMsgEncode(char key, unsigned long session)
{
	//string msg;

	//TODO：鼠标信令编码实现

	//encodedMsgQueue.push(msg);

	//ReleaseSemaphore(hsCtrlMsgEncoded, 1, NULL);
}

bool ctrlMsgHandler::getEncodedMsg(string & msg)
{
	if (encodedMsgQueue.empty())
	{
		return false;
	}

	msg = encodedMsgQueue.front();

	encodedMsgQueue.pop();

	return true;
}
