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
int ctrlMsgHandler::decodeMsg(string msg)
{
	int msgType;

	/*
		�������Ǽ������������������ȡ���Ự��
	*/

	int session;

	allMsgHead *publicHead = (allMsgHead *)&msg;

	msgType = publicHead->payloadType;

	session = publicHead->session;

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

	return session;
}

string ctrlMsgHandler::encodeMsg(imgHead head, int imgSize, int session)
{
	string msg;

	imgMsgHead msgHead;

	//��д�ṹ��
	msgHead.imgChannels = head.channels;
	msgHead.imgCols = head.cols;
	msgHead.imgRows = head.rows;
	msgHead.imgType = head.imgType;

	msgHead.payloadType = OPENCV_MAT;

	msgHead.msgType = IMG_HEAD;

	msgHead.msgFlag = 0x03;

	msgHead.msgSize = imgSize + 96 / sizeof(BYTE);

	//ת���ַ���
	msg = to_string(msgHead.msgSize) + to_string(msgHead.msgType) + to_string(msgHead.msgFlag) + to_string(msgHead.imgRows) + to_string(msgHead.imgCols) + to_string(msgHead.imgChannels) + to_string(msgHead.imgType) + to_string(msgHead.payloadType);

	//���Ϲ���ͷ
	msg = encodePublicHead(IMG_MSG, session, msg.size()) + msg;

	return msg;
}

string ctrlMsgHandler::encodePublicHead(int payloadType, int session, int size)
{
	string msg;

	allMsgHead publicHead;

	//��д�ṹ��
	publicHead.cks = 0;
	publicHead.payloadType = payloadType;
	publicHead.session = session;
	publicHead.msgSize = 64 / sizeof(BYTE) + size;

	//ת���ַ���
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