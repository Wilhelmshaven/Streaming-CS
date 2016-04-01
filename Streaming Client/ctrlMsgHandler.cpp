/*--Author������--*/

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

void ctrlMsgHandler::keyboardMsgEncode(char key)
{
	string msg;
	msg.resize(sizeof(keyboardMsg));

	auto head = (keyboardMsg *)msg.c_str();

	head->msgType = KB_ENG;

	head->isPressed = KB_CLICK;

	/*
		תUnicodeΪ������
		https://msdn.microsoft.com/en-us/library/dd375731(v=vs.85).aspx
	*/
	head->virtualCode = htons(VkKeyScan(key));

	head->unicode = htonl(key);

	encodedMsgQueue.push(msg);

	ReleaseSemaphore(hsCtrlMsgEncoded, 1, NULL);
}

void ctrlMsgHandler::mouseMsgEncode(char key)
{
	//string msg;

	//TODO������������ʵ��

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
