/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	�ص�I/O��Ҫ�õ��Ľṹ��
	��ʱ��¼IO���ݣ������û��ͳһ��׼
*/
typedef struct
{
	// ÿһ���ص�I/O���������Ҫ��һ��
	WSAOVERLAPPED overlapped;  

	WSABUF wsaBuf;   

	// ��ӦWSABUF��Ļ�����   
	string buffer;

	// ��־����ص�I/O��������ʲô�ģ�����Accept/Recv��
	int operationType;

	// ���յ���������
	DWORD bytesRecv;        

	DWORD flags;   

} PER_IO_DATA, *LPPER_IO_DATA;

/*
	�ṹ�����ƣ�PER_HANDLE_DATA
	�ṹ��洢����¼�����׽��ֵ����ݣ��������׽��ֵı������׽��ֵĶ�Ӧ�Ŀͻ��˵ĵ�ַ��
	�ṹ�����ã��������������Ͽͻ���ʱ����Ϣ�洢���ýṹ���У�֪���ͻ��˵ĵ�ַ�Ա��ڻطá�
*/
typedef struct
{
	// ���I/O������ʹ�õ�Socket
	SOCKET clientSocket; 

	// ...��ַ��Ϣ
	SOCKADDR_IN clientAddr; 

}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

/*
	�����̵߳Ĳ����ṹ��

	��Ա��
	HANDLE comp�����
	SOCKET sock��Socket
*/
typedef struct acptThreadParam
{
	HANDLE comp;
	SOCKET sock;
};

typedef struct stringSocketMsg
{
	string msg;
	SOCKET socket;
};

/*
	���Ӵ�������������

	���ܣ�����������������Ϣ����

	ʹ�ã�
	
	int srvConfig(int port = 8554)�����÷�����������Ĭ�Ϸ���ֵΪ0

	int startServer()�������������������������ӣ�����ֵΪwinsock��listen�����ķ���ֵ

	static bool getRTSPMsg(string &msg, SOCKET &socket)�����ڣ���ȡRTSP��������hsRTSPMsgArrived�ź�����

	static bool getCtrlMsg(string &msg, SOCKET &socket)�����ڣ���ȡ������������hsCtrlMsgArrived�ź�����

	void sendMessage(string msg, SOCKET socket)����ڣ������������͸�����Ϣ

	bool isSocketAlive(SOCKET clientSocket)�����ĳ���׽����Ƿ񻹻
*/
class cnctHandler
{
public:

	WSADATA wsaData;

	static cnctHandler *getInstance();           //���ص���

	//���÷���������
	int srvConfig(int port = 8554);

	//������������������������
	int startServer();

	//���ڣ���ȡRTSP��������hsRTSPMsgArrived�ź�����
	static bool getRTSPMsg(string &msg, SOCKET &socket);

	//���ڣ���ȡ������������hsCtrlMsgArrived�ź�����
	static bool getCtrlMsg(string &msg, SOCKET &socket);

	//���ڣ���ȡHTTP��������hsWebMsgArrived�ź�����
	static bool getHTTPMsg(string &msg, SOCKET &socket);

	//��ڣ������������͸�����Ϣ
	void sendMessage(string msg, SOCKET socket);

	//���ĳ���׽����Ƿ񻹻
	bool isSocketAlive(SOCKET clientSocket);

	~cnctHandler();                           

private:

	/*
		��ʱ���У����յ������������
		���������壨�����ϸ�ʽ�ģ���Ϣ������֮
	*/

	static queue<stringSocketMsg> rtspQueue, ctrlQueue, webQueue;

	/*
		�߳����
	*/

	//�������߳̾��������ΪCPU����
	vector<HANDLE> workerThread;          

	//�̴߳�����
	static DWORD WINAPI workerThreadFunc(LPVOID lparam);     

	//����ר�Ÿ�������������߳�
	static DWORD WINAPI acptThread(LPVOID lparam);                  

	//ָ���������˿�
	string srvPort;  

	//�洢ϵͳ��Ϣ�Ľṹ
	SYSTEM_INFO sysInfo;  

	//��������ַ�ṹ
	SOCKADDR_IN srvAddr;    

	//��ɶ˿�
	HANDLE completionPort; 

	//Ψһ��SOCKET
	SOCKET srvSocket;	

	//����CPU���������������߳�
	void buildThread();        

	acptThreadParam *param;

	/*
		����ģʽ
	*/
	static cnctHandler *instance;            
	cnctHandler();                          

	cnctHandler(const cnctHandler &);
	cnctHandler & operator = (const cnctHandler &);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo Garbo;
};