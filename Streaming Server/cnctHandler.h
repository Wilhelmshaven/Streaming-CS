#pragma once在·
#include "CommonHeaders.h"

//重叠I/O需要用到的类，临时记录IO数据，这个并没有统一标准
typedef struct PER_IO_OPERATION_DATA
{
	WSAOVERLAPPED overlapped;  // 每一个重叠I/O网络操作都要有一个
	WSABUF databuff;           // 
	char buffer[BUF_SIZE];     // 对应WSABUF里的缓冲区
	DWORD bytesRecv;           // 接收到的数据量
	DWORD flags;               //
	int operationType;         // 标志这个重叠I/O操作是做什么的，例如Accept/Recv等
};

/**
* 结构体名称：PER_HANDLE_DATA
* 结构体存储：记录单个套接字的数据，包括了套接字的变量及套接字的对应的客户端的地址。
* 结构体作用：当服务器连接上客户端时，信息存储到该结构体中，知道客户端的地址以便于回访。
**/
typedef struct PER_HANDLE_DATA
{
	SOCKET clientSocket;       // 这个I/O操作所使用的Socket
	SOCKADDR_IN clientAddr;    // ...地址信息
};

class cnctHandler
{
public:
	WSADATA wsaData;

	//一系列返回信息的函数
	static cnctHandler *getInstance();           //返回单例

	//设置服务器参数
	void srvConfig(string port);

	//启动服务器（直接进行到监听）
	int startServer();

	//线程相关
	vector<HANDLE> workerThread;               //工作者线程，数量为CPU核数*2
	static UINT workerThreadFunc();            //线程处理函数
	static UINT acptThread();                  //用于专门负责处理传入请求的线程

	~cnctHandler();                            //必要的清理工作还是要写出来的

private:
	static cnctHandler *instance;              //单例
	cnctHandler();                             //构造函数

	static SOCKET srvSocket;                          //唯一的SOCKET，避免乱改

	//禁止拷贝构造以及赋值
	cnctHandler(const cnctHandler &);
	cnctHandler & operator = (const cnctHandler &);

	//析构处理
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
	string srvPort;            //指定服务器端口
	SYSTEM_INFO sysInfo;       //存储系统信息的结构	
	SOCKADDR_IN srvAddr;       //服务器地址结构

	static HANDLE completionPort;     //完成端口

	//Function
	void getSystemInfo();          //获取系统信息（最主要是CPU核数，以便创建Worker线程）
	void buildThread();            //根据CPU核数建立工作者线程
	bool isSocketAlive();
};