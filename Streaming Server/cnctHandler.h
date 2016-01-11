#pragma once在·
#include "CommonHeaders.h"

class cnctHandler
{
public:
	WSADATA wsaData;

	//一系列返回信息的函数
	static cnctHandler *getInstance();           //返回单例

	//设置服务器参数
	void srvConfig(int port);

	//启动服务器（直接进行到监听）
	void startServer();

	//线程相关
	vector<HANDLE> workerThread;             //工作者线程，数量为CPU核数*2
	static UINT workerThreadFunc();          //线程处理函数

	~cnctHandler();                   //必要的清理工作还是要写出来的

private:
	static cnctHandler *instance;              //单例
	cnctHandler();                             //构造函数

	SOCKET srvSocket;                          //唯一的SOCKET，避免乱改

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

	HANDLE completionPort;     //完成端口

	//Function
	void getSystemInfo();          //获取系统信息（最主要是CPU核数，以便创建Worker线程）
	void buildThread();            //根据CPU核数建立工作者线程
	bool isSocketAlive(SOCKET s);

	
};