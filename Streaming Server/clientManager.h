/*--Author：李宏杰--*/
#pragma once
#include "CommonHeaders.h"

/*
	为每一个会话存储对应数据的结构体
	包括：套接字，端口号，传输模式（TCP/UDP）
*/
typedef struct PerClientData
{
	SOCKET socket;

	unsigned int streamingPort;

	bool enableUDP;

	//图像缩放比率（仅针对摄像头设计）
	double scaleFactorX;
	double scaleFactorY;

	//是否播放
	bool play;

	//帧率（实际上是发送频率，摄像头的帧率是不变的）
	unsigned int frameRate;

	//Additional attribute

	bool threshold;
	bool isDrawn;
	double axesX;
	double axesY;
};

/*
	客户端列表类

	使用：

	bool addClient(unsigned long session, SOCKET socket, int port, bool enableUDP)：插入客户端

	bool searchClient(unsigned long session)：查询客户端是否存在
	bool searchClient(SOCKET socket)：查询客户端是否存在

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

	/*
		为摄像头增加的相关内容
	*/

	//遍历用起始迭代器
	map<unsigned long, PerClientData>::iterator getIteratorStart();

	//遍历用终点迭代器
	map<unsigned long, PerClientData>::iterator getIteratorEnd();

	//改变客户端配置：是否播放
	bool changePlayFactor(SOCKET index);

	//改变客户端配置：缩放率（模拟不同分辨率传输）
	bool changePlayFactor(SOCKET index, double scaleFactor, bool changeX, bool changeY);

	//改变客户端配置：帧率（参数为正负偏移量）
	bool changePlayFactor(SOCKET index, int offset);

	//
	bool changeImageFactor(SOCKET index);

	bool drawEllipse(SOCKET index);
	bool setEllipseX(SOCKET index, bool axes);
	bool setEllipseY(SOCKET index, bool axes);

private:

	/*
		Key-Value MAP
	*/

	map<unsigned long, PerClientData> clientList;
	
	map<SOCKET, unsigned long> socToSessList;

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