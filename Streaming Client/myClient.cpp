#include "cnctHandler.h"
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

//================================= MAIN =================================//
int main(int argc, char *argv[])
{
	/*------------------------------��������--------------------------*/
	//��ʼ��Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//��÷�����ʵ��
	cnctHandler *mySrv = cnctHandler::getInstance();

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
	cnctHandler *mySrv = cnctHandler::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	//���÷�������Ϣ����rtsp������
	rtsp->setHandler(mySrv->getDisplayAddr(), MAKEWORD(1, 0), mySrv->getStreamPort(), true);

	//����Socket���ճ�ʱ����������̫�ã��ŵ�Ȼ�������ڻ���������ģʽ���ͻ���ô
	int recvTimeMax = 5000;  //5s
	//setsockopt(mySrv->getSocket(), SOL_SOCKET, SO_RCVTIMEO, (char *)recvTimeMax, sizeof(int));

	//�����շ��������߳�
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)sendMsgThread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)recvMsgThread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)rtspHeartBeat, NULL, NULL, NULL);     //�������Play�Ժ����
	
	//�����߳�
	SetEvent(hRTSPBeginEvent);

	//�ȴ�����
	WaitForSingleObject(hCloseClientEvent, INFINITE);

	CloseHandle(sendMsgThread);
	CloseHandle(recvMsgThread);
	CloseHandle(rtspHeartBeat);

	return 0;
}

//RTSP������߳�
void sendMsgInThread(int msgType); //���������߳��ڵĺ�����һ��С��װ������->����->������
UINT sendMsgThread()
{
	//��ȡ����
	cnctHandler *mySrv = cnctHandler::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	sendRecvMutex = CreateMutex(NULL, TRUE, NULL);          //�շ�����ʹ�û����ź����������ó�ʱ
	WaitForSingleObject(hRTSPBeginEvent,INFINITE);          //�ȴ���ʼָ��    

	string sendBuf, recvBuf;        //�շ�����
	int bytesSent;                  //���͵��ֽ���

	sendMsgInThread(DESCRIBE);      //���ȷ���DESCRIBE����

	//������
	ReleaseMutex(sendRecvMutex);
	Sleep(1000);                    //ֻ��Ϊ�˷�����ԣ���ͬ
	WaitForSingleObject(sendRecvMutex, INFINITE);

	sendMsgInThread(SETUP);             //Ȼ����Setup����

	//������
	ReleaseMutex(sendRecvMutex);
	Sleep(1000);
	WaitForSingleObject(sendRecvMutex, INFINITE);

	sendMsgInThread(PLAY);                                   //Ȼ����Playָ��
	hBeatStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);  //���������¼�

	//������
	ReleaseMutex(sendRecvMutex);
	Sleep(1000);
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
	cnctHandler *mySrv = cnctHandler::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	string sendBuf;        //�շ�����
	int bytesSent;         //���͵��ֽ���

	sendBuf = rtsp->encodeMsg(msgType);
	bytesSent = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.length(), NULL);

	if (bytesSent == 0)
	{
		//����ʧ��
		cout << "Error in sending msg:" << sendBuf << endl;
		SetEvent(hCloseClientEvent);           //Ŀǰ���õķ�ʽ��ֱ�ӹرտͻ��ˣ�TODO���ĳ��л���һ��������
	}
	else
	{
		//���ͳɹ������������Ϣ
		cout << "Bytes sent:" << bytesSent << endl;
		cout << "Msg sent:" << endl << sendBuf << endl;
	}
}

//RTSP��������߳�
UINT recvMsgThread()
{
	//��ȡ����
	cnctHandler *mySrv = cnctHandler::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	string recvBuf;        //�շ�����
	int errCode;           //�������
	
	WaitForSingleObject(hRTSPBeginEvent, INFINITE);         //�ȴ��߳̿�ʼ�¼�

	while (1)
	{
		WaitForSingleObject(sendRecvMutex, INFINITE);       //�ȴ��շ���������һ��һ����
		WaitForSingleObject(hCloseClientEvent, 0);          //�������¼��Ƿ��ѱ�����

		recvBuf.clear();
		recvBuf.resize(BUF_SIZE);

		//���մ𸴣�Ŀǰͨ�����ó�ʱ�����������ã�
		recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
		recvBuf = recvBuf.substr(0, recvBuf.find('\0'));         //��������
		errCode = rtsp->decodeMsg(recvBuf);
		if (errCode != 200)
		{
			cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
			SetEvent(hCloseClientEvent);   //����һ�������¼������������ǽ������л���ʲô
			ReleaseMutex(sendRecvMutex);
			break;
		}
		else
		{
			cout << "Msg recv: " << endl << recvBuf << endl;

			//��������¼���������������PLAY��������OK������������
			if (hBeatStartEvent != NULL)SetEvent(hBeatStartEvent);   
		}

		ReleaseMutex(sendRecvMutex);
		Sleep(1000);               //���ڲ���
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
	cnctHandler *mySrv = cnctHandler::getInstance();
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