#include "myServer.h"
#include "cnctHandler.h"
#include "imageQueue.h"

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
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//��õ���
	cnctHandler *mySrv = cnctHandler::getInstance();










	return 0;
}

UINT sendImgQueue()
{
	return 0;
}

