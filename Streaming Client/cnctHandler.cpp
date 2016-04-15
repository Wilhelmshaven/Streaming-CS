/*--Author：李宏杰--*/

#include "cnctHandler.h"

//网络模块：标记有消息需要发送
HANDLE hsNewSendMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctInput);

//网络模块：标记接收到了新的RTP消息
HANDLE hsNewRTPMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctRTPOutput);

//网络模块：标记接收到了新的RTSP消息
HANDLE hsNewRTSPMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctRTSPOutput);

cnctHandler *cnctHandler::instance = new cnctHandler(srvSettingFile);

queue<string> cnctHandler::sendMsgQueue, cnctHandler::recvRTPQueue, cnctHandler::recvRTSPQueue;

/*
	构造函数（传入文件名为参数，否则读取公共头中指定的文件路径）

	默认设置：
	1、启动Winsock
	2、把服务器信息设为本地（loaclhost）
	3、读取服务器配置文件
*/
cnctHandler::cnctHandler(string fileName)
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);

	srvAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &srvAddr.sin_addr);
	srvAddr.sin_port = htons(80);

	//读入配置文件
	if (readFile(fileName))
	{
		cout << "读取配置文件成功！" << endl;
	}
	else
	{
		cout << "读取配置文件失败！" << endl;
	}
}

cnctHandler* cnctHandler::getInstance()
{
	return instance;
}

/*
	读取配置文件，返回值代表是否成功读取
*/
bool cnctHandler::readFile(string fileName)
{
	//以只读文件流形式打开文件
	fstream srvSettings;
	srvSettings.open(fileName, ios_base::in);

	if (!srvSettings.is_open())
	{
		//文件未打开或未找到，报错
		cout << "找不到配置文件： " << srvSettingFile << " !" << endl;

		return false;
	}
	
	//输入缓存
	string buf;   

	//标签中的内容
	string label;  

	//读到文件结尾
	//Todo：以后可考虑从特定位置读起/保留当前读取状态/直接存到内存之类的处理方法
	while (!srvSettings.eof())
	{
		//Tips. 这里一次要读一整行的，避免有空格之类坑爹的东西
		getline(srvSettings, buf);

		//1、解析服务器组信息
		if (buf.find("<serverList>") != string::npos)
		{
			//只要是服务器就全部读进来

			while (buf.find("</serverList>") == string::npos)
			{
				getline(srvSettings, buf);

				if (buf.find("<server>") != string::npos)
				{
					//读取单个服务器的信息
					srvInfo serverInfo;

					ZeroMemory(&serverInfo, sizeof(srvInfo));

					while (buf.find("</server>") == string::npos)
					{
						/*
							取出标签中的内容
						*/

						getline(srvSettings, buf);
						label = buf.substr(buf.find('<') + 1, buf.find('>') - buf.find('<') - 1);

						/*
							根据标签中的内容填充结构体
						*/
						
						fillInfo(label, buf, serverInfo);
					}

					myServerList.push_back(serverInfo);
				}
			}
		}

		//2、解析其它（待添加）
		//Todo：...
	}
	
	return true;
}

/*
	填充服务器信息结构体
*/
void cnctHandler::fillInfo(string label, string buf, srvInfo & serverInfo)
{
	string value = buf.substr(buf.find('>') + 1, buf.rfind('<') - buf.find('>') - 1);

	do
	{
		if (label == "srvName")
		{
			serverInfo.srvName = value;

			break;
		}

		if (label == "address")
		{
			serverInfo.address = value;

			break;
		}

		if (label == "protocol")
		{
			serverInfo.protocol = value;

			break;
		}

		if (label == "hostName")
		{
			serverInfo.hostName = value;

			/*
				解析域名/IP地址并记录
				Todo：这个不合法怎么解决呢？
			*/
			int errCode = getaddrinfo(value.c_str(), NULL, NULL, &(serverInfo.srv));

			if (errCode != 0)
			{
				serverInfo.isValid = false;
				//cout << "配置文件中包含非法域名，错误代码：" << WSAGetLastError() << endl;
				//errHandler(201)
			}
			else
			{
				serverInfo.isValid = true;
			}

			break;
		}

		if (label == "port")
		{
			//直接填入Winsock地址结构	
			serverInfo.port = htons(stoul(value));

			break;
		}
	} while (0);
}

/*
	连接服务器：自动从头开始选择第一个可用的（能连接上）的服务器进行连接

	Todo：增加切换服务器功能
*/
int cnctHandler::connectServer()
{
	//返回值：若成功会改为0，否则为-1（代表无可用服务器）或其它值（能连接上但出错，此时代表connect函数返回的错误码，参见MSDN）
	int isConnected = -1;

	auto iter = myServerList.begin();
	auto end = myServerList.end();

	//从服务器链表中读出相关信息，并挨个尝试连接
	while (iter != end)
	{
		//信息不合法
		if (!iter->isValid)
		{
			++iter;
			continue;
		}

		cout << "尝试连接服务器（75秒超时）：" << iter->address << endl;

		/*
			填写结构信息与端口
			Tips. sockaddr和sockaddr_in是一回事，只不过前者更通用（直接把几个属性凑成字符串了）
		*/

		sockaddr_in srvAddrTmp = *(sockaddr_in *)iter->srv->ai_addr;

		srvAddrTmp.sin_port = iter->port;

		/*
			连接服务器。这里是可能超时的，默认超时时间是75秒
			Todo：万一真的阻塞了，这可不行啊，不能让人等很久很久啊
		*/

		//Tips. 必须先使用临时变量存数据去连接，Connect函数估计比较复杂，会因为权限问题无法连接的
		SOCKET socketTmp = socket(AF_INET, SOCK_STREAM, 0);

		isConnected = connect(socketTmp, (sockaddr *)&srvAddrTmp, sizeof(srvAddrTmp));

		//在控制台输出相应信息
		if (isConnected == 0)
		{
			cout << "--连接成功" << endl;

			myServer = (*iter);

			srvSocket = socketTmp;

			//显示服务器信息
			//showSrvInfo();

			//启动收发线程
			startThread();

			break;
		}
		else
		{
			cout << "--连接失败，Winsock错误代码：" << WSAGetLastError() << endl;

			//切换下一组服务器，直到无备选服务器（指针为空）
			++iter;
		}
		
	}

	//如果一组都不成功，就输出无可用服务器
	if (isConnected != 0)
	{		
		cout << "无可用服务器." << endl;
	}

	return isConnected;
}

string cnctHandler::getDisplayAddr()
{
	return myServer.address;
}

void cnctHandler::sendMessage(string msg)
{
	sendMsgQueue.push(msg);

	ReleaseSemaphore(hsNewSendMsg, 1, NULL);
}

bool cnctHandler::getRTPMessage(string & msg)
{
	if (recvRTPQueue.empty())
	{
		return false;
	}

	msg = recvRTPQueue.front();

	recvRTPQueue.pop();

	return true;
}

bool cnctHandler::getRTSPMessage(string & msg)
{
	if (recvRTSPQueue.empty())
	{
		return false;
	}

	msg = recvRTSPQueue.front();

	recvRTSPQueue.pop();

	return true;
}

cnctHandler::~cnctHandler()
{
	closesocket(srvSocket);
}

void cnctHandler::startThread()
{
	threadParam *param = new threadParam;

	param->socket = srvSocket;

	CreateThread(NULL, NULL, recvThread, param, NULL, NULL);

	CreateThread(NULL, NULL, sendThread, param, NULL, NULL);

	//设置Socket接收超时为5秒，避免阻塞太久（嗯当然我们现在还是用阻塞模式，客户端么）
	int recvTimeMax = 5000;
	setsockopt(srvSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)recvTimeMax, sizeof(int));
}

DWORD cnctHandler::sendThread(LPVOID lparam)
{
	threadParam *param = (threadParam *)lparam;

	SOCKET socket = param->socket;

	string msg;
	int bytesSent;

	while (1)
	{
		WaitForSingleObject(hsNewSendMsg, INFINITE);

		if (!sendMsgQueue.empty())
		{
			msg = sendMsgQueue.front();

			sendMsgQueue.pop();

			bytesSent = send(socket, msg.c_str(), msg.length(), NULL);

			if (bytesSent > 0)
			{
				cout << "Send Message:" << endl << msg << endl;
			}
		}
	}

	return 0;
}

/*
	接收线程，有很多需要修改的地方，主要是收图片
*/
DWORD cnctHandler::recvThread(LPVOID lparam)
{
	threadParam *param = (threadParam *)lparam;

	SOCKET socket = param->socket;

	string msg;
	
	string img;

	int bytesRecv;

	while (1)
	{
		msg.resize(BUF_SIZE);

		//TODO：这里可能有问题，待测试
		bytesRecv = recv(socket, (char *)msg.data(), BUF_SIZE, NULL);
		
		if (bytesRecv >= 0)
		{
			msg = msg.substr(0, bytesRecv);
		}
		else
		{
			cout << WSAGetLastError() << endl;
			continue;
		}

		//这里分析接收到的信息类型，塞入相应的队列并激活信号量
		
		if (msg.find("RTSP") != string::npos)
		{
			//RTSP数据
			cout << "Recv:" << endl << msg << endl;

			recvRTSPQueue.push(msg);

			ReleaseSemaphore(hsNewRTSPMsg, 1, NULL);
		}

		//RTP数据
		if (msg[0] == '$')
		{
			recvRTPQueue.push(img);

			ReleaseSemaphore(hsNewRTPMsg, 1, NULL);

			//New Img
			img.clear();
			img += msg;
				
		}
		else
		{
			img += msg;
		}
	}

	return 0;
}