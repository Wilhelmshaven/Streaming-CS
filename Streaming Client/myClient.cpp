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

HANDLE hCloseClientEvent = CreateEvent(NULL, TRUE, FALSE, NULL);      //�¼����رտͻ��ˣ���ֵΪNULL
HANDLE hRTSPBeginEvent = CreateEvent(NULL, TRUE, FALSE, NULL);        //�¼�������RTSP�շ�
HANDLE hBeatStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);        //�¼�������RTSP�����̣߳���ֵΪNULL
HANDLE sendRecvMutex = NULL;             //rtsp�շ������ź���

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

	WaitForSingleObject(hCloseClientEvent, INFINITE);
	system("pause");
	return 0;
}

//RTSP�����߳�
UINT rtspHandleThread()
{
	//���߳�
	UINT sendMsgThread();       //������Ϣ
	UINT recvMsgThread();       //������Ϣ
	UINT rtspHeartBeat();       //����
	
	//��ȡ����
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	//���÷�������Ϣ����rtsp������
	rtsp->setHandler(mySrv->getDisplayAddr(), MAKEWORD(1, 0), mySrv->getStreamPort(), true);

	//����Socket���ճ�ʱ����������̫�ã��ŵ�Ȼ�������ڻ���������ģʽ���ͻ���ô
	int recvTimeMax = 5000;  //5s
	setsockopt(mySrv->getSocket(), SOL_SOCKET, SO_RCVTIMEO, (char *)recvTimeMax, sizeof(int));

	//�����շ��������߳�
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)sendMsgThread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)recvMsgThread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)rtspHeartBeat, NULL, NULL, NULL);     //�������Play�Ժ����
	
	//�����߳�
	SetEvent(hRTSPBeginEvent);

	WaitForSingleObject(hCloseClientEvent, INFINITE);

	CloseHandle(sendMsgThread);
	CloseHandle(recvMsgThread);
	CloseHandle(rtspHeartBeat);

	//string sendBuf, recvBuf;        //�շ�����
	//int errCode;                    //�������
	//int bytesSent;
	//recvBuf.resize(BUF_SIZE);

	////���ǵ������ͳ�ʱ����ͷ�������ٿ��������̣߳��Ա���õؿ��ơ�
	////���ȷ���DESCRIBE����
	//sendBuf = rtsp->encodeMsg(DESCRIBE);
	//bytesSent = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.length(), NULL);

	////���մ𸴣���д�ɷ�������
	//recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
	//errCode = rtsp->decodeMsg(recvBuf);
	//if (errCode != 200)
	//{
	//	cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
	//	SetEvent(hCloseClientEvent);   //����һ�������¼������������ǽ������л���ʲô
	//}

	////Ȼ����Setup����
	//sendBuf = rtsp->encodeMsg(SETUP);
	//send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

	////���մ𸴣���д�ɷ�������
	//recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
	//errCode = rtsp->decodeMsg(recvBuf);
	//if (errCode != 200)
	//{
	//	cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
	//	SetEvent(hCloseClientEvent);   //����һ�������¼������������ǽ������л���ʲô
	//}

	////Ȼ����Playָ��
	//sendBuf = rtsp->encodeMsg(PLAY);
	//send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

	////���մ𸴣���д�ɷ�������
	//recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
	//errCode = rtsp->decodeMsg(recvBuf);
	//if (errCode != 200)
	//{
	//	cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
	//	SetEvent(hCloseClientEvent);   //����һ�������¼������������ǽ������л���ʲô
	//}

	////�����Ļ��������������߳�

	return 0;
}

//RTSP������߳�
void sendMsgInThread(int msgType); //���������߳��ڵĺ�����һ��С��װ������->����->������
UINT sendMsgThread()
{
	//��ȡ����
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	//�շ�����ʹ�û����ź����������ó�ʱ
	sendRecvMutex = CreateMutex(NULL, TRUE, NULL);

	string sendBuf, recvBuf;        //�շ�����
	int bytesSent;                  //���͵��ֽ���

	sendMsgInThread(DESCRIBE);          //���ȷ���DESCRIBE����

	//������
	ReleaseMutex(sendRecvMutex);
	WaitForSingleObject(sendRecvMutex, INFINITE);

	sendMsgInThread(SETUP);             //Ȼ����Setup����

	//������
	ReleaseMutex(sendRecvMutex);
	WaitForSingleObject(sendRecvMutex, INFINITE);

	sendMsgInThread(PLAY);              //Ȼ����Playָ��

	//������
	ReleaseMutex(sendRecvMutex);
	WaitForSingleObject(sendRecvMutex, INFINITE);

	//�ȴ�����ָ������̣߳�����Ҫ�ٷ�ʲô�ˣ�
	WaitForSingleObject(hCloseClientEvent, INFINITE);
	ReleaseMutex(sendRecvMutex);

	sendMsgInThread(TEARDOWN);//TEARDOWN���Ͽ�����

	return 0;
}

//���������߳��ڵĺ�����һ��С��װ������->����->������
void sendMsgInThread(int msgType)
{
	//��ȡ����
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	string sendBuf, recvBuf;        //�շ�����
	int bytesSent;                  //���͵��ֽ���

	sendBuf = rtsp->encodeMsg(msgType);
	bytesSent = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.length(), NULL);

	if (bytesSent == 0)
	{
		cout << "Error in sending msg:" << sendBuf << endl;
		SetEvent(hCloseClientEvent);                     //Ŀǰ���õķ�ʽ��ֱ�ӹرտͻ���
	}
	else
	{
		cout << "Bytes sent:" << bytesSent << endl;
		cout << "Msg sent:" << sendBuf << endl;
	}
}

//RTSP��������߳�
UINT recvMsgThread()
{
	//��ȡ����
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	string recvBuf;        //�շ�����
	int errCode;           //�������
	recvBuf.resize(BUF_SIZE);

	while (1)
	{
		WaitForSingleObject(sendRecvMutex, INFINITE);
		WaitForSingleObject(hCloseClientEvent, 0);

		//���մ𸴣�Ŀǰͨ�����ó�ʱ�����������ã�
		recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
		errCode = rtsp->decodeMsg(recvBuf);
		if (errCode != 200)
		{
			cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
			SetEvent(hCloseClientEvent);   //����һ�������¼������������ǽ������л���ʲô
			ReleaseMutex(sendRecvMutex);
			break;
		}

		ReleaseMutex(sendRecvMutex);
	}

	return 0;
}

//RTSP�����̣߳�30�뷢��һ��GET_PARAMETER����ֱ�������ͻ����¼�������
//�����
UINT rtspHeartBeat()
{
	//�ȴ������ź�
	WaitForSingleObject(hBeatStartEvent, INFINITE);

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
		if (WaitForSingleObject(hCloseClientEvent, 0))break;     //����Ƿ���Ҫ����	 

		if (WaitForSingleObject(heartBeatTimer, 1000))           //ÿ����һ��
		{
			sendBuf = rtsp->encodeMsg(GET_PARAMETER);
			errCode = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

			if (errCode != 200)
			{
				//�������
				cout << "�������ͳ���������룺" << errCode << " " << rtsp->getErrMsg(errCode) << endl;
			}

			//������ȡ�ظ���
		}	
	}

	return 0;
}