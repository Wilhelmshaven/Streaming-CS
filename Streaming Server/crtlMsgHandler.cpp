/*--Author������--*/

#include "crtlMsgHandler.h"

//�����м��
#include "middleWare.h"

ctrlMsgHandler* ctrlMsgHandler::instance = new ctrlMsgHandler;

ctrlMsgHandler * ctrlMsgHandler::getInstance()
{
	return instance;
}

/*
	����
*/
void ctrlMsgHandler::decodeMsg(int msgType, string msg)
{
	switch (msgType)
	{
	case KB_MSG:
	{
		//����������װ���������ṹ��
		keyboardMsg *kbMsg = (keyboardMsg *)&msg;

		//ת��������Ϊ�ַ�
		char key;
		key = MapVirtualKey(kbMsg->unicode, MAPVK_VK_TO_CHAR);

		//TODO�������ַ����м��������Ⱦ��




		break;
	}
	case MOUSE_MSG:
		break;
	default:
		break;
	}
}
