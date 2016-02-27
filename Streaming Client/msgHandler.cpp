/*--Author������--*/

#include "msgHandler.h"

//�����м��ģ��
#include "middleWare.h"

msgHandler* msgHandler::instance = new msgHandler;

msgHandler * msgHandler::getInstance()
{
	return instance;
}

string msgHandler::ctrlMsgEncode()
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

	mwPlayCtrl* middleWare = mwPlayCtrl::getInstance();

	//���м���л�ȡUnicode��BUT����֤�������а���
	unionMsg.msgHead.unicode = middleWare->getCtrlKey();

	//תUnicodeΪ������
	unionMsg.msgHead.virtualCode = VkKeyScan(unionMsg.msgHead.unicode);

	unionMsg.msgHead.is_pressed = KB_CLICK;
	unionMsg.msgHead.msgType = KB_ENG;

	msg = unionMsg.msg;

	//�����ź�����ʾ����һ��ģ��ɵ���
	ReleaseSemaphore(hsMsgHandler, 1, NULL);

	//Todo�������ֽ���תΪ�����ֽ���
	//��ʵ������߶���ת����Ҳûɶ�¡���

	return msg;
}

msgHandler::msgHandler()
{

}