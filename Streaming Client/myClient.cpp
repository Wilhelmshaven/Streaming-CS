/*--Author������--*/

#include "myClient.h"

//�����м��
#include "middleWare.h"

namespace myHandle
{
	HANDLE heCloseClient;

	HANDLE heESCPressed;
}
using namespace myHandle;

void initServer()
{
	//���ﻹҪ��ϸ���ǣ����ڸ������һ���Գ���ĸо���
	//TODO�����Ӹ�λ����

	heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::clientClose);

	heESCPressed = CreateEvent(NULL, TRUE, FALSE, syncManager::ESCPressed);
}

// ר�����ǰ����ʾ��Ϣ
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     ����ʽ��ý������֡����ƽ̨���汾v1.0     =" << endl;
	cout << "=            143020085211001 ����            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--��ʼ����..." << endl;
	cout << "--���������ļ�..." << endl;
};
const myMessage myMsg;

//================================= MAIN =================================//
int main(int argc, char *argv[])
{
	initServer();

	middleWare *mWare = middleWare::getInstance();

	mWare->startMiddleWare();

	WaitForSingleObject(heESCPressed, INFINITE);

	/*
		��ʼ������
	*/

	mWare->shutdownAll();

	return 0;
}