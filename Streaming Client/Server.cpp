#include "Server.h"

//直接初始化
Server *Server::instance = new Server(srvSettingFile);

//一系列返回相关信息的函数
Server* Server::getInstance()
{
	return instance;
}

SOCKET Server::getSocket()
{
	return srvSocket;
}

serverList* Server::getServerInfo()
{
	return mySrvList;
}

string Server::getDisplayAddr()
{
	return displayAddr;
}

int Server::getStreamPort()
{
	return srvAddr.sin_port;
}

//获取标签里的内容，填到相应的变量中
void serverList::getLabelMsg(string name, string buf)
{
	int index = 0;

	//这里要根据常量表添加修改了，似乎无法对修改封闭啊
	//所以……要遵从设计好的协议！
	if (name == "type")index = 1;
	if (name == "protocol")index = 2;
	if (name == "hostname")index = 3;
	if (name == "port")index = 4;
	if (name == "display")index = 5;

	//提取标签中的内容
	srvArgs[index] = buf.substr(buf.find('>') + 1, buf.rfind('<') - buf.find('>') - 1);
}

//返回对应值
string serverList::getCfgByIndex(int index)
{
	return srvArgs[index];
}

//构造函数
Server::Server(string file)
{
	//VAR init
	fileName = file;
	mySrvList = new serverList;
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Initialize as a local Real-time Streaming server (Default Settings)
	srvAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &srvAddr.sin_addr);
	srvAddr.sin_port = htons(80);

	//读入配置文件
	readFile();
};

//析构函数
Server::~Server()
{
	closesocket(srvSocket);
}

//读取配置文件，返回是否成功读取
bool Server::readFile()
{
	fstream srvSettings;
	srvSettings.open(fileName, ios_base::in);  //只读就可以

	string buf;    //输入缓存
	string label;  //标签中的内容

	if (!srvSettings.is_open())
	{
		//文件未打开或未找到，报错
		cout << "找不到配置文件： " << srvSettingFile << " !" << endl;
		return false;
	}
	else
	{
		//读到文件结尾，以后可考虑从特定位置读起/保留当前读取状态/直接存到内存之类的处理方法
		while (!srvSettings.eof())
		{
			//这里一次要读一整行的，避免有空格之类坑爹的东西
			getline(srvSettings, buf);
			
			//1、解析服务器组信息
			if (buf.find("<serverList>") != string::npos)
			{
				//只要是服务器就全部读进来，创建一个服务器链表
				serverList *newServer = mySrvList, *nextServer = NULL;

				while (buf.find("</serverList>") == string::npos)
				{
					getline(srvSettings, buf);
					if (buf.find("<server>") != string::npos)
					{
						//读取单个服务器的信息
						nextServer = new serverList;
						while (buf.find("</server>") == string::npos)
						{
							getline(srvSettings, buf);
							label = buf.substr(buf.find('<') + 1, buf.find('>') - buf.find('<') - 1);  //取出标签中的内容

							nextServer->getLabelMsg(label, buf);               //根据标签中的内容填充
						}

						//添加新的服务器节点，建立双向链表
						newServer->next = nextServer;
						nextServer->prev = newServer;
						nextServer = nextServer->next;
						newServer = newServer->next;
					}
				}
			}

			//2、解析其它（待添加）
		}
	}

	return true;
};

//连接服务器：自动从头开始选择第一个可用的（能连接上）的服务器进行连接
//后续扩展：切换服务器功能
int Server::connectServer()
{
	serverList *waitToCnct = mySrvList->next;

	int isConnected = -1;
	//从服务器链表中读出相关信息，并挨个尝试连接
	while (waitToCnct != NULL)
	{
		//必须先使用临时变量存数据去连接，Connect函数估计比较复杂，会因为权限问题无法连接的

		//拼接完整的连接路径
		string serverTmp;
		serverTmp = waitToCnct->getCfgByIndex(protocol) + "://" + waitToCnct->getCfgByIndex(hostName) + "/" + waitToCnct->getCfgByIndex(displayRoute);
		if (serverTmp.length() > 5)cout << "尝试连接服务器（75秒超时）：" << serverTmp << endl;

		//解析域名，这个不合法怎么解决呢？
		addrinfo *res;
		int errorCode = getaddrinfo(waitToCnct->getCfgByIndex(hostName).c_str(), NULL, NULL, &res);

		if (errorCode != 0)
		{
			cout << "域名解析失败，非法域名。错误代码：" << WSAGetLastError() << endl;
		}
		else
		{
			//填写结构信息，端口自己写
			//TMD， sockaddr和sockaddr_in是一回事，只不过前者更通用（直接把几个属性凑成字符串了）
			sockaddr_in srvAddrTmp = *(sockaddr_in *)res->ai_addr;
			srvAddrTmp.sin_port = htons(atoi(waitToCnct->getCfgByIndex(port).c_str()));

			inet_ntop(srvAddrTmp.sin_family, &srvAddrTmp.sin_addr, (char *)waitToCnct->srvArgs[hostAddr].data(), 17); //回填解析结果

			//这里是可能超时的，默认超时时间是75秒
			//To do list：万一真的阻塞了，这可不行啊，不能让人等很久很久啊
			SOCKET socketTmp = socket(AF_INET, SOCK_STREAM, 0);
			isConnected = connect(socketTmp, (sockaddr *)&srvAddrTmp, sizeof(srvAddrTmp)); 	

			if (isConnected == 0)
			{
				cout << "--连接成功" << endl;
				srvAddr = srvAddrTmp;
				displayAddr = serverTmp;
				srvSocket = socketTmp;

				break;
			}
			else
			{
				cout << "--连接失败，错误代码：" << WSAGetLastError() << endl;
				waitToCnct = waitToCnct->next;  //切换下一组服务器
			}
		}
	}

	if (isConnected != 0)
	{
		//如果一组都不成功，就输出无可用服务器
		cout << "无可用服务器." << endl;
	}

	return isConnected;
}