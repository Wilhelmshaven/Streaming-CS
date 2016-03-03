/*--Author������--*/

#include "ctrlMsgHandler.h"

ctrlMsgHandler* ctrlMsgHandler::instance = new ctrlMsgHandler;

ctrlMsgHandler * ctrlMsgHandler::getInstance()
{
	return instance;
}

/*
	����
*/
void ctrlMsgHandler::decodeMsg(string msg)
{
	int msgType;

	/*
		�������Ǽ���������������
	*/

	allMsgHead *publicHead = (allMsgHead *)&msg;

	msgType = publicHead->payloadType;

	delete(publicHead);

	/*
		�����������ʹ���
	*/
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

		ctrlKeyQueue.push(key);

		ReleaseSemaphore(hsCtrlMsg, 1, NULL);

		delete(kbMsg);

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