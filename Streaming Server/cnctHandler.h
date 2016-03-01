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
	char buffer[BUF_SIZE];     

	// ���յ���������
	DWORD bytesRecv;        

	DWORD flags;   

	// ��־����ص�I/O��������ʲô�ģ�����Accept/Recv��
	int operationType;         

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

/*
	���Ӵ�������������

	���ܣ�

	ʹ�ã�
	
	int srvConfig(int port)�����÷�����������Ĭ�Ϸ���ֵΪ0

	int startServer()�������������������������ӣ�����ֵΪwinsock��listen�����ķ���ֵ
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

	//���ĳ���׽����Ƿ񻹻
	bool isSocketAlive(SOCKET clientSocket);

	~cnctHandler();                           

private:

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