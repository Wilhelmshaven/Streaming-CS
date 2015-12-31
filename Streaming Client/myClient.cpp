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


UINT rtspHandleThread();              //rtsp�����߳�

HANDLE hCloseClient = CreateEvent(NULL, TRUE, FALSE, NULL);      //�¼����رտͻ��ˣ���ֵΪNULL
HANDLE sendRecvMutex;             //rtsp�շ������ź���

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
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)rtspHandleThread, NULL, NULL, NULL);          //rtsp�����߳�
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
UINT rtspHandleThread()
{
	//�շ����������߳�
	UINT sendMsgThread();
	UINT recvMsgThread();
	UINT rtspHeartBeat();

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)sendMsgThread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)recvMsgThread, NULL, NULL, NULL);

	//�������Play�Ժ󴴽�
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)rtspHeartBeat, NULL, NULL, NULL);

	//��ȡ����
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	//���÷�������Ϣ����rtsp������
	rtsp->setHandler(mySrv->getDisplayAddr(), MAKEWORD(1, 0), mySrv->getStreamPort(), true);

	string sendBuf, recvBuf;        //�շ�����
	int errCode;                    //�������
	int bytesSent;
	recvBuf.resize(BUF_SIZE);

	//���ǵ������ͳ�ʱ����ͷ�������ٿ��������̣߳��Ա���õؿ��ơ�
	//���ȷ���DESCRIBE����
	sendBuf = rtsp->encodeMsg(DESCRIBE);
	bytesSent = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.length(), NULL);

	//���մ𸴣���д�ɷ�������
	recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
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
	recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
	errCode = rtsp->decodeMsg(recvBuf);
	if (errCode != 200)
	{
		cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
		SetEvent(hCloseClient);   //����һ�������¼������������ǽ������л���ʲô
	}

	//Ȼ����Playָ��
	sendBuf = rtsp->encodeMsg(PLAY);
	send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

	//���մ𸴣���д�ɷ�������
	recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
	errCode = rtsp->decodeMsg(recvBuf);
	if (errCode != 200)
	{
		cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
		SetEvent(hCloseClient);   //����һ�������¼������������ǽ������л���ʲô
	}

	//�����Ļ��������������߳�

	return 0;
}

UINT sendMsgThread()
{
	//�շ�����ʹ�û����ź����������ó�ʱ
	sendRecvMutex = CreateMutex(NULL, TRUE, NULL);

	return 0;
}

UINT recvMsgThread()
{
	return 0;
}

UINT rtspHeartBeat()
{
	//�ȴ������ź�


	//��ȡ����
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	//��ʱ�����
	HANDLE heartBeatTimer = CreateWaitableTimer(NULL, FALSE, NULL);     //������ʱ�����Զ�����
	LARGE_INTEGER timeCnt;
	timeCnt.QuadPart = 0;                                               //0s��������ʱ����ֵ������
	SetWaitableTimer(heartBeatTimer, &timeCnt, 30000, NULL, NULL, 0);   //��������Ϊ���ڣ���λ����.һ�����ý��ϵͳ60s��ʱ
	
	string sendBuf;        //�շ�����
	int errCode;           //�������

	while (1)
	{
		if (WaitForSingleObject(hCloseClient, 0))break;              //����Ƿ���Ҫ����	                  
		if (!WaitForSingleObject(heartBeatTimer, 10000))continue;    //�ȴ�10��

		sendBuf = rtsp->encodeMsg(GET_PARAMETER);
		errCode = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

		if (errCode != 200)
		{
			//�������
			cout << "�������ͳ���������룺" << errCode << " " << rtsp->getErrMsg(errCode) << endl;
		}

		//������ȡ�ظ���
	}

	return 0;
}