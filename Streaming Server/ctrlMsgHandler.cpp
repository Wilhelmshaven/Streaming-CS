/*--Author������--*/

#include "ctrlMsgHandler.h"

namespace ctrlNS
{
	//���������ģ�飺������������ϣ����м������ת����Ⱦ��
	HANDLE ctrlMsgDecoded = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgDecoded);

	//���������ģ�飺������������ϣ����м������ת������ģ��
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
	����
*/
void ctrlMsgHandler::decodeMsg(SOCKET index, string msg)
{
	/*
		��ȡ���Ự������������
	*/

	auto *aHead = (allMsgHead *)msg.c_str();

	unsigned int payloadType, session;

	payloadType = aHead->payloadType;

	session = ntohl(aHead->session);

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
		//TODO�����������ڲ�д����

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
//	//��д�ṹ��
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
//	//���Ϲ���ͷ
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

	//��д�ṹ��
	publicHead->cks = 0;
	publicHead->payloadType = payloadType;
	publicHead->session = htonl(session);
	publicHead->msgSize = htons(sizeof(allMsgHead) + size);

	return msg;
}