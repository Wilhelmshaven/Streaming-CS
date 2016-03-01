/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	重叠I/O需要用到的结构体
	临时记录IO数据，这个并没有统一标准
*/
typedef struct
{
	// 每一个重叠I/O网络操作都要有一个
	WSAOVERLAPPED overlapped;  

	WSABUF wsaBuf;   

	// 对应WSABUF里的缓冲区
	char buffer[BUF_SIZE];     

	// 接收到的数据量
	DWORD bytesRecv;        

	DWORD flags;   

	// 标志这个重叠I/O操作是做什么的，例如Accept/Recv等
	int operationType;         

} PER_IO_DATA, *LPPER_IO_DATA;

/*
	结构体名称：PER_HANDLE_DATA
	结构体存储：记录单个套接字的数据，包括了套接字的变量及套接字的对应的客户端的地址。
	结构体作用：当服务器连接上客户端时，信息存储到该结构体中，知道客户端的地址以便于回访。
*/
typedef struct
{
	// 这个I/O操作所使用的Socket
	SOCKET clientSocket; 

	// ...地址信息
	SOCKADDR_IN clientAddr; 

}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

/*
	监听线程的参数结构体

	成员：
	HANDLE comp：句柄
	SOCKET sock：Socket
*/
typedef struct acptThreadParam
{
	HANDLE comp;
	SOCKET sock;
};

/*
	连接处理器（单例）

	功能：

	使用：
	
	int srvConfig(int port)：设置服务器参数，默认返回值为0

	int startServer()：启动服务器并监听传入连接，返回值为winsock中listen函数的返回值
*/
class cnctHandler
{
public:
	WSADATA wsaData;

	static cnctHandler *getInstance();           //返回单例

	//设置服务器参数
	int srvConfig(int port = 8554);

	//启动服务器并监听传入连接
	int startServer();

	//检查某个套接字是否还活动
	bool isSocketAlive(SOCKET clientSocket);

	~cnctHandler();                           

private:

	/*
		线程相关
	*/

	//工作者线程句柄，数量为CPU核数
	vector<HANDLE> workerThread;          

	//线程处理函数
	static DWORD WINAPI workerThreadFunc(LPVOID lparam);     

	//用于专门负责处理传入请求的线程
	static DWORD WINAPI acptThread(LPVOID lparam);                  

	//指定服务器端口
	string srvPort;  

	//存储系统信息的结构
	SYSTEM_INFO sysInfo;  

	//服务器地址结构
	SOCKADDR_IN srvAddr;    

	//完成端口
	HANDLE completionPort; 

	//唯一的SOCKET
	SOCKET srvSocket;	

	//根据CPU核数建立工作者线程
	void buildThread();        

	acptThreadParam *param;

	/*
		单例模式
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