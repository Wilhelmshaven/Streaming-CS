#pragma once
#include "CommonHeaders.h"

//�ص�I/O��Ҫ�õ����࣬��ʱ��¼IO���ݣ������û��ͳһ��׼
typedef struct PER_IO_OPERATION_DATA
{
	WSAOVERLAPPED overlapped;  // ÿһ���ص�I/O���������Ҫ��һ��

	WSABUF databuff;           // 
	char buffer[BUF_SIZE];     // ��ӦWSABUF��Ļ�����
	DWORD bytesRecv;           // ���յ���������
	DWORD flags;               //
	int operationType;         // ��־����ص�I/O��������ʲô�ģ�����Accept/Recv��
};

/**
* �ṹ�����ƣ�PER_HANDLE_DATA
* �ṹ��洢����¼�����׽��ֵ����ݣ��������׽��ֵı������׽��ֵĶ�Ӧ�Ŀͻ��˵ĵ�ַ��
* �ṹ�����ã��������������Ͽͻ���ʱ����Ϣ�洢���ýṹ���У�֪���ͻ��˵ĵ�ַ�Ա��ڻطá�
**/
typedef struct PER_HANDLE_DATA
{
	SOCKET clientSocket;       // ���I/O������ʹ�õ�Socket
	SOCKADDR_IN clientAddr;    // ...��ַ��Ϣ
};

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
	vector<HANDLE> workerThread;               //�������̣߳�����ΪCPU����*2
	static UINT workerThreadFunc();            //�̴߳�����
	static UINT acptThread();                  //����ר�Ÿ�������������߳�

	~cnctHandler();                            //��Ҫ������������Ҫд������

private:
	static cnctHandler *instance;              //����
	cnctHandler();                             //���캯��

	static SOCKET srvSocket;                          //Ψһ��SOCKET�������Ҹ�

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

	static HANDLE completionPort;     //��ɶ˿�

	//Function
	int getSystemInfo();          //��ȡϵͳ��Ϣ������Ҫ��CPU�������Ա㴴��Worker�̣߳�
	int buildThread();            //����CPU���������������߳�
	bool isSocketAlive();
};