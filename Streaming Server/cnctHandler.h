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
	string buffer;

	// 标志这个重叠I/O操作是做什么的，例如Accept/Recv等
	int operationType;

	// 接收到的数据量
	DWORD bytesRecv;        

	DWORD flags;   

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

typedef struct stringSocketMsg
{
	string msg;
	SOCKET socket;
};

/*
	连接处理器（单例）

	功能：负责处理传入连接与信息发出

	使用：
	
	int srvConfig(int port = 8554)：设置服务器参数，默认返回值为0

	int startServer()：启动服务器并监听传入连接，返回值为winsock中listen函数的返回值

	static bool getRTSPMsg(string &msg, SOCKET &socket)：出口，获取RTSP信令。请监听hsRTSPMsgArrived信号量。

	static bool getCtrlMsg(string &msg, SOCKET &socket)：出口，获取控制信令。请监听hsCtrlMsgArrived信号量。

	void sendMessage(string msg, SOCKET socket)：入口，（单纯）发送给定信息

	bool isSocketAlive(SOCKET clientSocket)：检查某个套接字是否还活动
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

	//出口：获取RTSP信令。请监听hsRTSPMsgArrived信号量。
	static bool getRTSPMsg(string &msg, SOCKET &socket);

	//出口：获取控制信令。请监听hsCtrlMsgArrived信号量。
	static bool getCtrlMsg(string &msg, SOCKET &socket);

	//出口：获取HTTP信令。请监听hsWebMsgArrived信号量。
	static bool getHTTPMsg(string &msg, SOCKET &socket);

	//入口：（单纯）发送给定信息
	void sendMessage(string msg, SOCKET socket);

	//检查某个套接字是否还活动
	bool isSocketAlive(SOCKET clientSocket);

	~cnctHandler();                           

private:

	/*
		临时队列，接收到的信令按类别入队
		对于无意义（不符合格式的）消息，丢弃之
	*/

	static queue<stringSocketMsg> rtspQueue, ctrlQueue, webQueue;

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