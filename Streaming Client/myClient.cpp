#include "Server.h"
#include "myClient.h"
#include "rtspHandler.h"

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
	cout << "--���������ļ�..." << endl;
};
const myMessage myMsg;


UINT rtspInteract();                  //rtsp�����߳�
UINT rtspHeartBeat();                 //rtsp�����߳�

HANDLE hCloseClient = CreateEvent(NULL, TRUE, FALSE, NULL);      //�¼����رտͻ��ˣ���ֵΪNULL


//================================= MAIN =================================//
int main(int argc, char *argv[])
{
	/*------------------------------��������--------------------------*/
	//��ʼ��Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//��÷�����ʵ��
	Server *mySrv = Server::getInstance();

	//Connect to server
	if (mySrv->connectServer() == 0)
	{
		/*------------------------------RTSP����--------------------------*/
		//�����Ȼ��Ҫ�������߳�����
		//���������½������̡߳�һ���ǿ��ƽ�����һ������������Ϊ���������߳̾ͺã���ά�����ӵ�
		//�̲߳���Ҫ�����ˣ���������Ҫʲô��ʲô
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)rtspInteract, NULL, NULL, NULL);          //rtsp�����߳�
	}
	else
	{
		//���ӷ�����ʧ��
	}

	WaitForSingleObject(hCloseClient, INFINITE);
	system("pause");
	return 0;
}

//RTSP�����߳�
UINT rtspControl(LPVOID lpParam)
{
	//��ȡ����
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	string sendBuf, recvBuf;        //�շ�����
	int errCode;                    //�������

	//���ȷ���DESCRIBE����
	sendBuf = rtsp->encodeMsg(DESCRIBE);
	send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

	//���մ𸴣���д�ɷ�������
	recv(mySrv->getSocket(), (char *)recvBuf.data(), BUF_SIZE, NULL);
	errCode = rtsp->decodeMsg(recvBuf);
	if (errCode != 200)
	{
		cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
		SetEvent(hCloseClient);   //����һ�������¼������������ǽ������л���ʲô
	}

	//Ȼ����Setup����
	sendBuf = rtsp->encodeMsg(SETUP);
	send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

	//���մ𸴣���д�ɷ�������

	return 0;
}