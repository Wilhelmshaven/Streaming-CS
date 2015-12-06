#include "CommonHeaders.h"

//重叠I/O需要用到的类，临时记录IO数据，这个并没有统一标准
class PER_IO_OPERATION_DATA
{
public:
	WSAOVERLAPPED overlapped;  // 每一个重叠I/O网络操作都要有一个
	WSABUF databuff;           // 
	char buffer[BUF_SIZE];     // 对应WSABUF里的缓冲区
	DWORD NumberOfBytesRecv;   // 接收到的数据量
	DWORD Flags;
	int OperationType;         // 标志这个重叠I/O操作是做什么的，例如Accept/Recv等
};

/**
* 结构体名称：PER_HANDLE_DATA
* 结构体存储：记录单个套接字的数据，包括了套接字的变量及套接字的对应的客户端的地址。
* 结构体作用：当服务器连接上客户端时，信息存储到该结构体中，知道客户端的地址以便于回访。
**/
class PER_HANDLE_DATA
{
public:
	SOCKET ClientSocket;// 这个I/O操作所使用的Socket
	SOCKADDR_IN ClientAddr;
};

class Comport
{
protected:
	char *ServerPort;          //指定服务器端口
	WSADATA wsaData;           //WINSOCK
	HANDLE CompletionPort;     //完成端口
	SYSTEM_INFO SysInfo;       //系统信息
	SOCKET srvSocket;          //流式SOCKET
	SOCKADDR_IN srvAddr;       //服务器地址结构
	bool IsSocketAlive(SOCKET s);

	friend DWORD WINAPI ServerWorkThread(LPVOID CompletionPortID);//工作者线程


public:
	Comport();
	void SetPort(char *port);                     // 设置服务器监听端口
	void BindSocket();                            // 绑定Socket
	void StartListen();                           // 开始监听
	~Comport();

};

class MsgHeader
{
public:
	BYTE type;                //1为请求文件大小，2为发送文件请求
	DWORD offsetLow;
	DWORD offsetHigh;

	MsgHeader(){};
	~MsgHeader(){};
};