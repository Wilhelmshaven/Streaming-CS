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
		char key = MapVirtualKey(kbMsg->unicode, MAPVK_VK_TO_CHAR);

		/*
			�����ַ����м��ת����Ⱦ��
		*/

		KBQueue.push(key);

		ReleaseSemaphore(hsKBCtrlMsg, 1, NULL);

		break;
	}
	case MOUSE_MSG:
	{
		//TODO�����������ڲ�д����

		break;
	}
	default:
		break;
	}
}

char ctrlMsgHandler::getKBCtrlKey()
{
	char key = 0;

	if (!KBQueue.empty())
	{
		key = KBQueue.front();

		KBQueue.pop();
	}

	return key;
}

char ctrlMsgHandler::getMouseCtrlKey()
{
	//TODO�����������ڲ�д����

	return 0;
}
