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
	//��ʼ��Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//����Socket;
	SOCKET streamingSocket = socket(AF_INET, SOCK_STREAM, 0);

	//��÷�����ʵ��
	Server *mySrv = Server::getInstance();


	//--Test code--//
	//in_addr address;
	//sockaddr_in ServerAddr;

	//ServerAddr.sin_family = AF_INET;
	//inet_pton(AF_INET, "192.168.1.101", &address);
	//ServerAddr.sin_addr = address;
	//ServerAddr.sin_port = htons(atoi("554"));

	//int flag = connect(streamingSocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr));//�����ǿ��ܳ�ʱ�ģ�Ĭ�ϳ�ʱʱ����75��

	//if (flag == 0)mySrv->setSocket(streamingSocket);
	//--End of test code--//

	//Connect to server
	if (mySrv->connectServer() == 0)
	{
		streamingSocket = mySrv->getSocket();
	}
	else
	{
		//���ӷ�����ʧ��
		return 0;
	}

	//RTSP����
	rtspHandler rtspModule;
	serverList *srvInfo = mySrv->getServerInfo();
	rtspModule.initRTSPmsg(mySrv->getDisplayAddr(), atoi(srvInfo->getConfigByIndex(port).c_str()), streamingSocket);

	//�����߳�
	rtspParam rtspPara;
	rtspPara.URI = mySrv->getDisplayAddr();
	rtspPara.Port = atoi(srvInfo->getConfigByIndex(port).c_str());
	rtspPara.socket = streamingSocket;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rtspControl, &rtspPara, 0, NULL);



	return 0;
}

//RTSP�����߳�
UINT rtspControl(LPVOID lpParam)
{
	rtspParam *param = (rtspParam *)lpParam;
	rtspHandler rtspMsg;            // RTSP�������
	int flag;

	//����RTSP������Դ��������Ӧ����
	rtspMsg.initRTSPmsg(param->URI, param->Port, param->socket);     // ��ʼ��
	flag = rtspMsg.play();                                           // ���ţ�����ɴ�OPTION��PLAY������͹���
	if (flag != 200)
	{
		cout << "Error in playing media." << endl;
		return 0;
	}

	// ����
	int timer = 0;
	while (1)
	{
		timer++;           //��ʱ��

		if (timer > 100000)
		{
			timer = 0;
			//rtspMsg.HeartBeat();
		}

		////�¼����ƽ���
		//if (WaitForSingleObject(hEvent_Close, 1) == WAIT_OBJECT_0)
		//{
		//	break;
		//}
	}

	return 0;
}