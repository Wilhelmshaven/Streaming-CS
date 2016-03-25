/*--Author������--*/

#include "ctrlMsgHandler.h"

//���������ģ�飺������������ϣ����м������ת����Ⱦ��
HANDLE hsCtrlMsgDecoded = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgDecoded);

//���������ģ�飺������������ϣ����м������ת������ģ��
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
	����
*/
void ctrlMsgHandler::decodeMsg(string msg)
{
	/*
		��ȡ���Ự������������
	*/

	auto *aHead = (allMsgHead *)msg.c_str();

	unsigned int payloadType, session;

	payloadType = aHead->payloadType;

	session = aHead->session;

	/*
		�����������ʹ���
	*/

	switch (payloadType)
	{
	case KB_MSG:
	{
		//����������װ���������ṹ�塣ע�⣺��string��ʵ���ıȽ����׸��Ұ�����
		auto kbMsg = (keyboardMsg *)(msg.c_str() + 8);

		//ת��������Ϊ�ַ�
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
		//TODO�����������ڲ�д����

		break;
	}
	default:
		break;
	}
}

void ctrlMsgHandler::encodeMsg(imgHead head, int imgSize, int session)
{
	string msg;

	msg.resize(sizeof(imgMsgHead));

	imgMsgHead *msgHead = (imgMsgHead *)msg.c_str();

	//��д�ṹ��
	msgHead->imgChannels = head.channels;
	msgHead->imgCols = head.cols;
	msgHead->imgRows = head.rows;
	msgHead->imgType = head.imgType;

	msgHead->payloadType = OPENCV_MAT;

	msgHead->msgType = IMG_HEAD;

	msgHead->msgFlag = 0x03;

	msgHead->msgSize = imgSize + sizeof(imgMsgHead);

	//���Ϲ���ͷ
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
	msg.resize(sizeof(allMsgHead));

	allMsgHead *publicHead = (allMsgHead *)msg.c_str();

	//��д�ṹ��
	publicHead->cks = 0;
	publicHead->payloadType = payloadType;
	publicHead->session = session;
	publicHead->msgSize = sizeof(allMsgHead) + size;

	return msg;
}