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

UINT rtspControl(LPVOID lpParam);




//================================= MAIN =================================//
int main(int argc, char *argv[])
{
	/*------------------------------��������--------------------------*/
	//��ʼ��Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//����Socket;
	SOCKET streamingSocket = socket(AF_INET, SOCK_STREAM, 0);

	//��÷�����ʵ��
	Server *mySrv = Server::getInstance();

	//Connect to server
	if (mySrv->connectServer() == 0)
	{
		streamingSocket = mySrv->getSocket();
	}
	else
	{
		//���ӷ�����ʧ��
		system("pause");
		return 0;
	}

	/*------------------------------RTSP����--------------------------*/


	system("pause");
	return 0;
}

//RTSP�����߳�
UINT rtspControl(LPVOID lpParam)
{

	return 0;
}