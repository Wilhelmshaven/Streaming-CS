#include "CommonHeaders.h"

//服务器链表结构定义，双向链表
class serverList
{
public:
	string getCfgByIndex(int index);   //获取对应参数

	string srvArgs[ServerArgc];      //参数数量，而详细的参数，见常量表：ServerInfoDefine

	serverList *prev;
	serverList *next;

	void getLabelMsg(string name, string buf);    //填写内容	
};

//Server类，包括读取配置文件，建立Winsock连接，单例类
class Server
{
public:
	WSADATA wsaData;

	//一系列返回信息的函数
	static Server *getInstance();     //返回服务器实例
	SOCKET getSocket();               //返回SOCKET实例
	serverList* getServerInfo();      //返回服务器链表头指针
	string getDisplayAddr();          //返回完整地址
	int getStreamPort();              //返回端口号

	int connectServer();              //连接服务器，成功返回0

	~Server();                        //必要的清理工作还是要写出来的

private:
	static Server *instance;              //单例
	Server(string file);                  //构造函数

	serverList *mySrvList;                //服务器链表，内嵌类

	SOCKET srvSocket;                     //唯一的SOCKET，避免乱改

	//禁止拷贝构造以及赋值
	Server(const Server &);
	Server & operator = (const Server &);

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
	string fileName;           //配置文件的文件名

	// 当前正在使用的服务器信息
	sockaddr_in srvAddr;       //地址结构（ipv4/6，IP，端口）
	string displayAddr;        //完整的播放地址，eg. http://www.rayion.com/desktop

	bool readFile();  // 读取配置文件
};