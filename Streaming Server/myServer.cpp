#include "myServer.h"

/////���ظ���ģ��/////

//1.����ͷ�ɼ�ģ�飺�ɼ�ͼ��
#include "camCap.h"

//2.ͼ�����ģ�飨����ͼƬ���棩����ͼ��װ��ͼ�����
#include "imageQueue.h"

//3.���Ӵ���ģ�飨Winsock��������ͼ���շ�����
#include "cnctHandler.h"

// ר�����ǰ����ʾ��Ϣ
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     ����ʽ��ý������֡����ƽ̨���汾v0.1     =" << endl;
	cout << "=            143020085211001 ����            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--��ʼ����..." << endl;
	cout << "--��������..." << endl;
};
const myMessage myMsg;


UINT sendImgQueue();                   //����ͼ���̣߳�

//================================= MAIN =================================//
int main(int argc, char *argv[])
{	
	/*------------------------------��������--------------------------*/
	//��ʼ��Winsock���汾2.2
	//WSADATA wsaData;
	//WSAStartup(MAKEWORD(2, 2), &wsaData);

	////��õ���
	//cnctHandler *mySrv = cnctHandler::getInstance();

	//mySrv->startServer();

	camCap test;

	test.startCapture(20);



	WaitForSingleObject(hSrvShutdown, INFINITE);

	return 0;
}

UINT sendImgQueue()
{
	return 0;
}

