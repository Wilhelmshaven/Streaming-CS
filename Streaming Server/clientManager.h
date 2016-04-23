#pragma once
#include "CommonHeaders.h"

/*
	客户端列表类

	使用：

	bool addClient(unsigned long session, SOCKET socket, int port, bool enableUDP)：插入客户端

	bool searchClient(unsigned long session)：查询客户端是否存在

	bool getClientInfo(unsigned long session, SOCKET &socket, int &port, bool &enableUDP)：获得客户端信息

	bool removeClient(unsigned long session)：移除客户端
*/
class clientManager
{
public:

	static clientManager *getInstance();

	//插入客户端
	bool addClient(unsigned long session, SOCKET socket, unsigned int port, bool enableUDP);

	//查询客户端是否存在
	bool searchClient(unsigned long session);
	bool searchClient(SOCKET socket);

	//输入会话号，获得客户端信息
	bool getClientInfo(unsigned long session, SOCKET &socket, unsigned int &port, bool &enableUDP);

	//移除客户端
	bool removeClient(unsigned long session);

	map<SOCKET, unsigned long> socToSessList;

private:

	/*
		为每一个会话存储对应数据的结构体
		包括：套接字，端口号，传输模式（TCP/UDP）
	*/
	typedef struct PerClientData
	{
		SOCKET socket;

		unsigned int streamingPort;

		bool enableUDP;
	};

	//Key-Value MAP
	map<unsigned long, PerClientData> clientList;
	

	/*
		Singleton
	*/

	clientManager();
	static clientManager *instance;

	clientManager(const clientManager &);
	clientManager & operator = (const clientManager &);

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