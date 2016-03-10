/*--Author������--*/

#include "myClient.h"

//�����м��
#include "middleWare.h"

//�������紦��ģ��
#include "cnctHandler.h"

//���ز�����ģ��
#include "cvPlayer.h"

//����RTSPģ��
#include "rtspHandler.h"

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

void sendMsgBackground(int msgType);

//================================= MAIN =================================//
int main(int argc, char *argv[])
{
	/*------------------------------��������--------------------------*/
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	cnctHandler *mySrv = cnctHandler::getInstance();

	if (mySrv->connectServer() == 0)
	{
		//�ɹ����ӷ�����

		/*
			�����м��
		*/

		mwPlayCtrl *middleWare = mwPlayCtrl::getInstance();

		middleWare->startMiddleWare();

		/*
			RTSP����
			���˲�д��RTSPģ�����ˣ���ֱ����������
			�����ͻ��˼�
		*/

		rtspHandler *rtspModule = rtspHandler::getInstance();

		string rtspMsg;

		int errCode;

		//1.����OPTIONS����
		sendMsgBackground(OPTIONS);

		if (WaitForSingleObject(hsIsRTSPOK, 5000) != WAIT_OBJECT_0)
		{
			SetEvent(hCloseClientEvent);

			goto endClient;
		}
		        
		//2.����DESCRIBE����
		sendMsgBackground(DESCRIBE);

		if (WaitForSingleObject(hsIsRTSPOK, 5000) != WAIT_OBJECT_0)
		{
			SetEvent(hCloseClientEvent);

			goto endClient;
		}

		//3.����SETUP����
		sendMsgBackground(SETUP);

		if (WaitForSingleObject(hsIsRTSPOK, 5000) != WAIT_OBJECT_0)
		{
			SetEvent(hCloseClientEvent);

			goto endClient;
		}

		//4.����PLAY����
		sendMsgBackground(PLAY);

		if (WaitForSingleObject(hsIsRTSPOK, 5000) != WAIT_OBJECT_0)
		{
			SetEvent(hCloseClientEvent);

			goto endClient;
		}

		//��ʼ����
		SetEvent(heStartHeartBeat);
	}
	else
	{
		//���ӷ�����ʧ��
		goto endClient;
	}

	WaitForSingleObject(hCloseClientEvent, INFINITE);

	/*
		����������
	*/

	//5.����TEARDOWN����
	sendMsgBackground(TEARDOWN);

endClient:

	ResetEvent(heStartHeartBeat);

	CloseHandle(hCloseClientEvent);

	system("pause");

	return 0;
}

/*
	����RTSP����ĺ�����һ��С��װ������->���ͣ�
*/
void sendMsgBackground(int msgType)
{
	cnctHandler *networkModule = cnctHandler::getInstance();

	rtspHandler *rtspModule = rtspHandler::getInstance();

	string rtspMsg;

	rtspMsg = rtspModule->encodeMsg(msgType);

	networkModule->sendMessage(rtspMsg);
}





////RTSP������߳�
//void sendMsgInThread(int msgType); //���������߳��ڵĺ�����һ��С��װ������->����->������
//UINT sendMsgThread()
//{
//	//��ȡ����
//	cnctHandler *mySrv = cnctHandler::getInstance();
//	rtspHandler *rtsp = rtspHandler::getInstance();
//
//	sendRecvMutex = CreateMutex(NULL, TRUE, NULL);          //�շ�����ʹ�û����ź����������ó�ʱ
//	WaitForSingleObject(hRTSPBeginEvent,INFINITE);          //�ȴ���ʼָ��    
//
//	string sendBuf, recvBuf;        //�շ�����
//	int bytesSent;                  //���͵��ֽ���
//
//	sendMsgInThread(DESCRIBE);      //���ȷ���DESCRIBE����
//
//	//������
//	ReleaseMutex(sendRecvMutex);
//	Sleep(1000);                    //ֻ��Ϊ�˷�����ԣ���ͬ
//	WaitForSingleObject(sendRecvMutex, INFINITE);
//
//	sendMsgInThread(SETUP);             //Ȼ����Setup����
//
//	//������
//	ReleaseMutex(sendRecvMutex);
//	Sleep(1000);
//	WaitForSingleObject(sendRecvMutex, INFINITE);
//
//	sendMsgInThread(PLAY);                                   //Ȼ����Playָ��
//	hBeatStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);  //���������¼�
//
//	//������
//	ReleaseMutex(sendRecvMutex);
//	Sleep(1000);
//	WaitForSingleObject(sendRecvMutex, INFINITE);
//
//	//�ȴ�����ָ������̣߳�����Ҫ�ٷ�ʲô�ˣ�
//	WaitForSingleObject(hCloseClientEvent, INFINITE);
//	ReleaseMutex(sendRecvMutex);
//
//	sendMsgInThread(TEARDOWN);//TEARDOWN���Ͽ�����
//
//	return 0;
//}
//

//
////RTSP��������߳�
//UINT recvMsgThread()
//{
//	//��ȡ����
//	cnctHandler *mySrv = cnctHandler::getInstance();
//	rtspHandler *rtsp = rtspHandler::getInstance();
//
//	string recvBuf;        //�շ�����
//	int errCode;           //�������
//	
//	WaitForSingleObject(hRTSPBeginEvent, INFINITE);         //�ȴ��߳̿�ʼ�¼�
//
//	while (1)
//	{
//		WaitForSingleObject(sendRecvMutex, INFINITE);       //�ȴ��շ���������һ��һ����
//		WaitForSingleObject(hCloseClientEvent, 0);          //�������¼��Ƿ��ѱ�����
//
//		recvBuf.clear();
//		recvBuf.resize(BUF_SIZE);
//
//		//���մ𸴣�Ŀǰͨ�����ó�ʱ�����������ã�
//		recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
//		recvBuf = recvBuf.substr(0, recvBuf.find('\0'));         //��������
//		errCode = rtsp->decodeMsg(recvBuf);
//		if (errCode != 200)
//		{
//			cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
//			SetEvent(hCloseClientEvent);   //����һ�������¼������������ǽ������л���ʲô
//			ReleaseMutex(sendRecvMutex);
//			break;
//		}
//		else
//		{
//			cout << "Msg recv: " << endl << recvBuf << endl;
//
//			//��������¼���������������PLAY��������OK������������
//			if (hBeatStartEvent != NULL)SetEvent(hBeatStartEvent);   
//		}
//
//		ReleaseMutex(sendRecvMutex);
//		Sleep(1000);               //���ڲ���
//	}
//
//	return 0;
//}
//
////RTSP�����̣߳�30�뷢��һ��GET_PARAMETER����ֱ�������ͻ����¼�������
////�����
//UINT rtspHeartBeat()
//{
//	//�ȴ������ź�
//	WaitForSingleObject(hBeatStartEvent, INFINITE);
//
//	//��ȡ����
//	cnctHandler *mySrv = cnctHandler::getInstance();
//	rtspHandler *rtsp = rtspHandler::getInstance();
//
//	//��ʱ�����
//	HANDLE heartBeatTimer = CreateWaitableTimer(NULL, FALSE, NULL);     //������ʱ�����Զ�����
//	LARGE_INTEGER timeCnt;
//	timeCnt.QuadPart = 0;                                               //0s��������ʱ����ֵ������
//	SetWaitableTimer(heartBeatTimer, &timeCnt, 30000, NULL, NULL, 0);   //��������Ϊ���ڣ���λ����.һ�����ý��ϵͳ60s��ʱ
//	
//	string sendBuf;        //�շ�����
//	int errCode;           //�������
//
//	while (1)
//	{
//		if (WaitForSingleObject(hCloseClientEvent, 0))break;     //����Ƿ���Ҫ����	 
//
//		if (WaitForSingleObject(heartBeatTimer, 1000))           //ÿ����һ��
//		{
//			sendBuf = rtsp->encodeMsg(GET_PARAMETER);
//			errCode = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);
//
//			if (errCode != 200)
//			{
//				//�������
//				cout << "�������ͳ���������룺" << errCode << " " << rtsp->getErrMsg(errCode) << endl;
//			}
//
//			//������ȡ�ظ���
//		}	
//	}
//
//	return 0;
//}