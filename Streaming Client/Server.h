#include "CommonHeaders.h"

//服务器链表结构定义
class serverList
{
public:
	string getConfigByIndex(int index);   //获取对应参数

	string srvArgs[6];      //参数数量，而详细的参数，见如上的常量表

	serverList *prev;
	serverList *next;

	void getLabelMsg(string name, string buf);    //填写内容	

};

//Server类，包括读取配置文件，建立Winsock连接，单例类
//To do list：增加域名解析功能，并且校验
class Server
{
public:
	WSADATA wsaData;

	static Server *getInstance()
	{
		return instance;
	}

	int connectServer();                 //连接服务器，成功返回0

	//返回SOCKET实例
	SOCKET getSocket()
	{
		return srvSocket;
	}           

	////Test Code
	//void setSocket(SOCKET in)
	//{
	//	srvSocket = in;
	//}

	//返回当前信息表
	serverList* getServerInfo()
	{
		return mySrvList->next;
	}

	string getDisplayAddr()
	{
		return displayAddr;
	}

protected:
	static Server *instance;              //单例
	Server(string file);                  //构造函数

	serverList *mySrvList;                //服务器链表，内嵌类

	SOCKET srvSocket;                     //避免乱改

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
	string fileName;           //配置文件名

	// 当前正在使用的服务器信息
	sockaddr_in srvAddr;       //地址结构（ipv4/6，IP，端口）
	string displayAddr;        //完整的播放地址，eg. http://www.rayion.com/desktop

	bool readFile();  // 读取配置文件
};