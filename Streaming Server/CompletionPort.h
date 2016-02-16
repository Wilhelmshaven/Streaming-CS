#include "CommonHeaders.h"

//�ص�I/O��Ҫ�õ����࣬��ʱ��¼IO���ݣ������û��ͳһ��׼
class PER_IO_OPERATION_DATA
{
public:
	WSAOVERLAPPED overlapped;  // ÿһ���ص�I/O���������Ҫ��һ��
	WSABUF databuff;           // 
	char buffer[BUF_SIZE];     // ��ӦWSABUF��Ļ�����
	DWORD NumberOfBytesRecv;   // ���յ���������
	DWORD Flags;
	int OperationType;         // ��־����ص�I/O��������ʲô�ģ�����Accept/Recv��
};

/**
* �ṹ�����ƣ�PER_HANDLE_DATA
* �ṹ��洢����¼�����׽��ֵ����ݣ��������׽��ֵı������׽��ֵĶ�Ӧ�Ŀͻ��˵ĵ�ַ��
* �ṹ�����ã��������������Ͽͻ���ʱ����Ϣ�洢���ýṹ���У�֪���ͻ��˵ĵ�ַ�Ա��ڻطá�
**/
class PER_HANDLE_DATA
{
public:
	SOCKET ClientSocket;// ���I/O������ʹ�õ�Socket
	SOCKADDR_IN ClientAddr;
};

class Comport
{
protected:
	char *ServerPort;          //ָ���������˿�
	WSADATA wsaData;           //WINSOCK
	HANDLE CompletionPort;     //��ɶ˿�
	SYSTEM_INFO SysInfo;       //ϵͳ��Ϣ
	SOCKET srvSocket;          //��ʽSOCKET
	SOCKADDR_IN srvAddr;       //��������ַ�ṹ
	bool IsSocketAlive(SOCKET s);

	friend DWORD WINAPI ServerWorkThread(LPVOID CompletionPortID);//�������߳�


public:
	Comport();
	void SetPort(char *port);                     // ���÷����������˿�
	void BindSocket();                            // ��Socket
	void StartListen();                           // ��ʼ����
	~Comport();

};

class MsgHeader
{
public:
	BYTE type;                //1Ϊ�����ļ���С��2Ϊ�����ļ�����
	DWORD offsetLow;
	DWORD offsetHigh;

	MsgHeader(){};
	~MsgHeader(){};
};