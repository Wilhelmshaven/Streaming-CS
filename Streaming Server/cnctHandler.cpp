/*--Author������--*/

#include "cnctHandler.h"

//����ģ�飺������µ�RTSP������Ϣ����
HANDLE hsRTSPMsgArrived = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::msgArrivedRTSP);

//����ģ�飺������µĿ���������Ϣ����
HANDLE hsCtrlMsgArrived = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::msgArrivedCtrl);

cnctHandler *cnctHandler::instance = new cnctHandler;

queue<stringSocketMsg> cnctHandler::rtspQueue, cnctHandler::ctrlQueue;

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

	//��ȡϵͳ��Ϣ������Ҫ��CPU�������Ա㴴��Worker�̣߳�
	GetSystemInfo(&sysInfo);

	//������ɶ˿�
	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, sysInfo.dwNumberOfProcessors);   

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

bool cnctHandler::getRTSPMsg(string &msg, SOCKET &socket)
{
	if (rtspQueue.empty())return false;
	
	msg = rtspQueue.front().msg;

	socket = rtspQueue.front().socket;

	rtspQueue.pop();
	
	return true;
}

bool cnctHandler::getCtrlMsg(string &msg, SOCKET &socket)
{
	if (ctrlQueue.empty())return false;
	
	msg = ctrlQueue.front().msg;

	socket = ctrlQueue.front().socket;

	ctrlQueue.pop();
	

	return true;
}

void cnctHandler::sendMessage(string msg, SOCKET socket)
{
	//bytesSent = send(socket, msg.c_str(), msg.length(), NULL);

	LPPER_IO_DATA ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));

	ZeroMemory(&(ioInfo->overlapped), sizeof(WSAOVERLAPPED));

	//����Ϊ���
	ioInfo->wsaBuf.len = msg.length();

	ioInfo->wsaBuf.buf = ioInfo->buffer;

	memcpy(ioInfo->buffer, msg.c_str(), msg.length());

	//����ģʽΪ����
	ioInfo->operationType = compSend;

	ioInfo->flags = 0;

	WSASend(socket, &(ioInfo->wsaBuf), 1, NULL, ioInfo->flags, &(ioInfo->overlapped), NULL);

	free(ioInfo);
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

	char incomingClient[17];

	while (true)
	{
		//��ʱSOCKET
		SOCKET acptSocket;
		SOCKADDR_IN clientAddr;
		int addrSize = sizeof(clientAddr);

		/*
			��������
			TODO�����Ǹ�����AcceptEx()��accept��WSAAccept��ͬ��������AcceptEx���첽����
		*/
		acptSocket = accept(srvSocket, (SOCKADDR*)&clientAddr, &addrSize);

		//����ͻ�����Ϣ
		PerHandleData = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		//PerHandleData->clientAddr = clientAddr;
		PerHandleData->clientSocket = acptSocket;
		memcpy(&(PerHandleData->clientAddr), &clientAddr, addrSize);

		inet_ntop(AF_INET, &(clientAddr.sin_addr), incomingClient, 17);
		cout << "New Client: " << incomingClient << endl;
		
		//�������׽��ֺ���ɶ˿ڹ���
		CreateIoCompletionPort((HANDLE)acptSocket, hCompletionPort, (DWORD)PerHandleData, 0);
		
		/*
			׼��һ���ص�I/O
		*/
		PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));

		//����������Ҫ��������ᵼ��GetQueuedCompletionStatus������
		ZeroMemory(&(PerIoData->overlapped), sizeof(WSAOVERLAPPED));   

		//����Ϊ���
		PerIoData->wsaBuf.len = BUF_SIZE;         

		PerIoData->wsaBuf.buf = PerIoData->buffer; 

		//����ģʽΪ����
		PerIoData->operationType = compRecv;	       
		
		PerIoData->flags = 0;

		//���½����׽�����Ͷ��һ�������첽WSARecv��WSASend���󣬲���������
		WSARecv(PerHandleData->clientSocket, &(PerIoData->wsaBuf), 1, &(PerIoData->bytesRecv), &(PerIoData->flags),
			&(PerIoData->overlapped), NULL);
	}

	free(param);
	free(PerHandleData);
	free(PerIoData);

	return 0;
}

/*
	�������߳�
*/
DWORD WINAPI cnctHandler::workerThreadFunc(LPVOID lparam)
{	
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
		buf.resize(BUF_SIZE);

		/*
			�õ���ɶ˿ڵ�״̬
			��������Ѿ���2-4�Ĳ��������ˣ��ڶ���ָʾ�յ����ֽ������������ǿͻ�����Ϣ�����ĸ�����ؽ�������
		*/
		GetQueuedCompletionStatus(hCompletionPort, &bytesTransferred, (LPDWORD)&handleInfo, (LPOVERLAPPED *)&ioInfo, INFINITE);

		clientSocket = handleInfo->clientSocket;

		string incomingIP;
		incomingIP.resize(16);
		inet_ntop(AF_INET, &(handleInfo->clientAddr.sin_addr.S_un.S_addr), (char *)incomingIP.data(), 16);

		/*
			������Ϣ����
		*/
		if (ioInfo->operationType == compRecv)
		{
			//EOF�����ӹر�
			if (bytesTransferred == 0)
			{
				cout << "Connection close: " << incomingIP << endl;

				closesocket(clientSocket);

				//free(handleInfo);

				//free(ioInfo);

				continue;
			}

			//��ʾ��Ϣ��Դ
			cout << "Recv message from " << incomingIP << endl;		

			/*
				��������
				���Ƚ�������ý�廹�ǿ��ƣ�Ȼ��������Ӧ�����У���󼤻��ź���
				ע���SOCKETһ�𶪽�ȥ����Ҫ��Ȼ�ط����ظ�˭����������
			*/

			memcpy((char *)buf.data(), ioInfo->buffer, bytesTransferred);
			//buf = ioInfo->buffer;
			buf = buf.substr(0, bytesTransferred);

			stringSocketMsg myMsg;
			myMsg.msg = buf;
			myMsg.socket = clientSocket;

			if (buf.find("RTSP") != string::npos)
			{
				cout << "Recv RTSP Msg:" << buf << endl;

				rtspQueue.push(myMsg);

				ReleaseSemaphore(hsRTSPMsgArrived, 1, NULL);
			}
			else
			{
				cout << "Recv CTRL Msg" << endl;

				ctrlQueue.push(myMsg);

				ReleaseSemaphore(hsCtrlMsgArrived, 1, NULL);
			}
		}

		/*
			׼����һ�����ӣ�Ͷ��һ��WSARecv
		*/

		//׼��һ���ص�I/O
		ioInfo = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_IO_DATA));

		ZeroMemory(&(ioInfo->overlapped), sizeof(WSAOVERLAPPED));

		//����Ϊ���
		ioInfo->wsaBuf.len = BUF_SIZE;

		ioInfo->wsaBuf.buf = ioInfo->buffer;

		//����ģʽΪ����
		ioInfo->operationType = compRecv;

		ioInfo->flags = 0;

		WSARecv(clientSocket, &(ioInfo->wsaBuf), 1, &(ioInfo->bytesRecv), &(ioInfo->flags), &(ioInfo->overlapped), NULL);		
	}

	return 0;
}