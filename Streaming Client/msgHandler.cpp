/*--Author������--*/

#include "msgHandler.h"

msgHandler* msgHandler::instance = new msgHandler;

msgHandler * msgHandler::getInstance()
{
	return instance;
}

string msgHandler::keyboardMsgEncode(char key)
{
	string msg;

	//�����壬��Ҫ�Ǳ��ڱ��룬������ṹһĿ��Ȼ��ͬʱҲ������תΪһ��
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

	//תUnicodeΪ������
	unionMsg.msgHead.virtualCode = VkKeyScan(unionMsg.msgHead.unicode);

	unionMsg.msgHead.is_pressed = KB_CLICK;
	unionMsg.msgHead.msgType = KB_ENG;

	msg = unionMsg.msg;

	//Todo�������ֽ���תΪ�����ֽ���
	//��ʵ������߶���ת����Ҳûɶ�¡���

	return msg;
}

string msgHandler::mouseMsgEncode()
{
	string msg;

	//TODO������������ʵ��

	return msg;
}

msgHandler::msgHandler()
{

}