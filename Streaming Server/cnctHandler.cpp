/*--Author������--*/

#include "cnctHandler.h"

//������Ϣ�м��ģ��
#include "middleWare.h"

cnctHandler *cnctHandler::instance = new cnctHandler;

queue<string> cnctHandler::rtspQueue, cnctHandler::ctrlQueue;
queue<SOCKET> cnctHandler::socketQueue;

/*
	���캯��

	ֵ��
	����Winsock
	������Ĭ�϶˿�Ϊ8554
	������ɶ˿�
	��ʼ��������������ַΪ�����ַ
	��ʼ��Socket
	��
*/
cnctHandler::cnctHandler()
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Ĭ�϶˿�
	srvPort = "8554";        

	//������ɶ˿�
	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);   

	//��ȡ����д��������Ϣ�����������ַ
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(stoi(srvPort, nullptr, 10));

	// ������ʽ�׽��֣�����Ҫʹ���ص�I/O���������Ҫʹ��WSASocket����ʼ��Socket
	srvSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	bind(srvSocket, (SOCKADDR *)&srvAddr, sizeof(srvAddr));
}

/*
	��������
*/
cnctHandler::~cnctHandler()
{
	delete param;

	//�˳���ɶ˿�
	for (int i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		PostQueuedCompletionStatus(completionPort, 0, 0, NULL);
	}

	CloseHandle(completionPort);

	shutdown(srvSocket, SD_BOTH);
	closesocket(srvSocket);

	WSACleanup();
}

cnctHandler* cnctHandler::getInstance()
{
	return instance;
}

/*
	���÷�����������˿ںţ�����ֵΪ0

	TODO������ֵΪ������
*/
int cnctHandler::srvConfig(int port)
{
	srvPort = to_string(port);

	return 0;
}

/*
	���ͻ����Ƿ�������Socket�����ز���ֵ
*/
bool cnctHandler::isSocketAlive(SOCKET clientSocket)
{
	string msg = "isAlive?";
	int byteSent;

	byteSent = send(clientSocket, msg.c_str(), msg.size(), NULL);

	if (byteSent == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/*
	�����������������߳�
*/
void cnctHandler::buildThread()
{
	//��ȡϵͳ��Ϣ������Ҫ��CPU�������Ա㴴��Worker�̣߳�
	GetSystemInfo(&sysInfo);

	int cntThread = sysInfo.dwNumberOfProcessors;

	workerThread.resize(cntThread);

	for (int i = 0; i < cntThread; i++) {

		// �����������������̣߳�������ɶ˿ڴ��ݵ����߳�
		workerThread[i] = CreateThread(NULL, NULL, workerThreadFunc, (LPVOID)completionPort, NULL, NULL);

		//CloseHandle(workerThread[i]);
	}
}

/*
	����������������
*/
int cnctHandler::startServer()
{	
	int status = 0;

	//����ɵ���ˣ�ǰ����ô�ຯ������˶�û���á���
	//��Ӧ�ø���������
	//�Լ�����װ����Ҳ����ʲô������

	buildThread();

	//��SOCKET����Ϊ����ģʽ
	status = listen(srvSocket, SOMAXCONN);                

	//����ָ��˵���������NEW�����򴴽��̺߳�ṹ�屻ɾ�������޷����롣�߳�����ɾ�����������ʹ�ã�
	param = new acptThreadParam;
	param->comp = completionPort;
	param->sock = srvSocket;

	//���������߳�
	CreateThread(NULL, NULL, acptThread, param, NULL, NULL);  

	return status;
}

string cnctHandler::getRTSPMsg()
{
	string msg;

	if (!rtspQueue.empty())
	{
		msg = rtspQueue.front();
		rtspQueue.pop();
	}

	return msg;
}

string cnctHandler::getCtrlMsg()
{
	string msg;

	if (!ctrlQueue.empty())
	{
		msg = ctrlQueue.front();
		ctrlQueue.pop();
	}

	return msg;
}

SOCKET cnctHandler::getSocket()
{
	SOCKET soc;

	if (!socketQueue.empty())
	{
		soc = socketQueue.front();

		socketQueue.pop();
	}

	return soc;
}

int cnctHandler::sendMessage(SOCKET socket, string msg)
{
	int bytesSent;

	bytesSent = send(socket, msg.c_str(), msg.length(), NULL);

	return bytesSent;
}

/*
	�����̣߳�ר�����ڽ��մ�������
*/
DWORD WINAPI cnctHandler::acptThread(LPVOID lparam)
{
	//���������
	acptThreadParam *param = (acptThreadParam *)lparam;
	HANDLE hCompletionPort = param->comp;
	SOCKET srvSocket = param->sock;

	//��IO����
	LPPER_IO_DATA PerIoData;         

	//���������
	LPPER_HANDLE_DATA PerHandleData;     

	//��ʱSOCKET
	SOCKET acptSocket;    

	SOCKADDR_IN clientAddr;
	int addrSize = sizeof(clientAddr);

	while (true)
	{
		//if (WaitForSingleObject(hSrvShutdown, 0))break;  //���������û�����Ǹ�����

		/*
			��������
			TODO�����Ǹ�����AcceptEx()��accept��WSAAccept��ͬ��������AcceptEx���첽����
		*/
		acptSocket = accept(srvSocket, (SOCKADDR*)&clientAddr, &addrSize);

		//����ͻ�����Ϣ
		PerHandleData = (LPPER_HANDLE_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_HANDLE_DATA));
		PerHandleData->clientAddr = clientAddr;
		PerHandleData->clientSocket = acptSocket;

		//�������׽��ֺ���ɶ˿ڹ���
		CreateIoCompletionPort((HANDLE)acptSocket, hCompletionPort, (DWORD)PerHandleData, 0);

		/*
			׼��һ���ص�I/O
		*/
		PerIoData = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_IO_DATA));	

		//����������Ҫ��������ᵼ��GetQueuedCompletionStatus������
		ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));   

		//����Ϊ���
		PerIoData->wsaBuf.len = BUF_SIZE;         

		PerIoData->wsaBuf.buf = PerIoData->buffer; 

		//����ģʽΪ����
		PerIoData->operationType = compRecv;	       
			
		//���½����׽�����Ͷ��һ�������첽WSARecv��WSASend���󣬲���������
		WSARecv(PerHandleData->clientSocket, &PerIoData->wsaBuf, 1, &PerIoData->bytesRecv, &PerIoData->flags,
			&(PerIoData->overlapped), NULL);
	}

	delete param;

	return 0;
}

/*
	�������߳�
*/
DWORD WINAPI cnctHandler::workerThreadFunc(LPVOID lparam)
{	
	mwMsg *mwMsgHandler = mwMsg::getInstance();

	//���������
	HANDLE hCompletionPort = (HANDLE)lparam;

	SOCKET clientSocket;

	DWORD bytesTransferred;
	DWORD flags;

	LPPER_HANDLE_DATA handleInfo;
	LPPER_IO_DATA ioInfo;
	
	string buf;

	while (true)
	{
		/*
			�õ���ɶ˿ڵ�״̬
			��������Ѿ���2-4�Ĳ��������ˣ��ڶ���ָʾ�յ����ֽ������������ǿͻ�����Ϣ�����ĸ�����ؽ�������
		*/
		GetQueuedCompletionStatus(hCompletionPort, &bytesTransferred, (LPDWORD)&handleInfo, (LPOVERLAPPED *)&ioInfo, INFINITE);

		//������������
		if (WaitForSingleObject(hSrvShutdown, 0))break;  //���������û�����Ǹ�����

		clientSocket = handleInfo->clientSocket;

		/*
			��������
			���Ƚ�������ý�廹�ǿ��ƣ�Ȼ��������Ӧ�����У���󼤻��ź���
			ע���SOCKETһ�𶪽�ȥ����Ҫ��Ȼ�ط����ظ�˭����������
		*/

		buf = ioInfo->buffer;

		if (buf.find("RTSP"))
		{
			rtspQueue.push(buf);

			socketQueue.push(handleInfo->clientSocket);

			ReleaseSemaphore(hsRTSPMsgArrived, 1, NULL);
		}
		else
		{
			ctrlQueue.push(buf);

			socketQueue.push(handleInfo->clientSocket);

			ReleaseSemaphore(hsCtrlMsgArrived, 1, NULL);
		}

		/*
			׼����һ�����ӣ�Ͷ��һ��WSARecv
		*/

		//׼��һ���ص�I/O
		ioInfo = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_IO_DATA));
		ZeroMemory(ioInfo, sizeof(ioInfo));

		//����Ϊ���
		ioInfo->wsaBuf.len = BUF_SIZE;    

		ioInfo->wsaBuf.buf = ioInfo->buffer; 

		//����ģʽΪ����
		ioInfo->operationType = compRecv;	       

		WSARecv(clientSocket, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
	}

	return 0;
}