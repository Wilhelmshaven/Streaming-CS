#include "myServer.h"
#include "CompletionPort.h"

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

UINT ListeningThread(LPVOID lpParam);                                                   //�����߳�
HANDLE workThread;                              // �߳̾��

int main(int argc, char *argv[])
{
	//��ʼ��Winsock���汾2.2
	workThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListeningThread, NULL, 0, NULL);


	return 0;
}

//�����߳�
UINT ListeningThread(LPVOID lpParam)
{
	Comport cPort;                                  // Winsock��ɶ˿ڶ���
	//cPort.SetPort(sp.port);
	cPort.BindSocket();
	cPort.StartListen();

	return 0;
}