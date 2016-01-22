#pragma once
#include "CommonHeaders.h"

//�ص�I/O��Ҫ�õ����࣬��ʱ��¼IO���ݣ������û��ͳһ��׼
typedef struct
{
	WSAOVERLAPPED overlapped;  // ÿһ���ص�I/O���������Ҫ��һ��

	WSABUF wsaBuf;             // 
	char buffer[BUF_SIZE];     // ��ӦWSABUF��Ļ�����
	DWORD bytesRecv;           // ���յ���������
	DWORD flags;               //
	int operationType;         // ��־����ص�I/O��������ʲô�ģ�����Accept/Recv��
} PER_IO_DATA, *LPPER_IO_DATA;

/**
* �ṹ�����ƣ�PER_HANDLE_DATA
* �ṹ��洢����¼�����׽��ֵ����ݣ��������׽��ֵı������׽��ֵĶ�Ӧ�Ŀͻ��˵ĵ�ַ��
* �ṹ�����ã��������������Ͽͻ���ʱ����Ϣ�洢���ýṹ���У�֪���ͻ��˵ĵ�ַ�Ա��ڻطá�
**/
typedef struct
{
	SOCKET clientSocket;       // ���I/O������ʹ�õ�Socket
	SOCKADDR_IN clientAddr;    // ...��ַ��Ϣ
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

class cnctHandler
{
public:
	WSADATA wsaData;

	//һϵ�з�����Ϣ�ĺ���
	static cnctHandler *getInstance();           //���ص���

	//���÷���������
	int srvConfig(string port);

	//������������ֱ�ӽ��е�������
	int startServer();

	//�߳����
	vector<HANDLE> workerThread;                                    //�������߳̾��������ΪCPU����
	static DWORD WINAPI workerThreadFunc(LPVOID lparam);            //�̴߳�����
	static DWORD WINAPI acptThread(LPVOID lparam);                  //����ר�Ÿ�������������߳�

	~cnctHandler();                            //��Ҫ������������Ҫд������

private:
	static cnctHandler *instance;              //����
	cnctHandler();                             //���캯��

	SOCKET srvSocket;                          //Ψһ��SOCKET�������Ҹ�

	//��ֹ���������Լ���ֵ
	cnctHandler(const cnctHandler &);
	cnctHandler & operator = (const cnctHandler &);

	//��������
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo Garbo;

	// VARs
	string srvPort;            //ָ���������˿�
	SYSTEM_INFO sysInfo;       //�洢ϵͳ��Ϣ�Ľṹ	
	SOCKADDR_IN srvAddr;       //��������ַ�ṹ

	HANDLE completionPort;     //��ɶ˿�

	//Function
	int getSystemInfo();                          //��ȡϵͳ��Ϣ������Ҫ��CPU�������Ա㴴��Worker�̣߳�
	int buildThread();                            //����CPU���������������߳�
	bool isSocketAlive(SOCKET clientSocket);      //���ĳ���׽����Ƿ񻹻
};

//�����̵߳Ĳ����ṹ��
typedef struct acptThreadParam
{
	HANDLE comp;
	SOCKET sock;
};