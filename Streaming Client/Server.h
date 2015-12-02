#include "CommonHeaders.h"

//class Server
//{
//public:
//	sockaddr_in srvAddr;
//	string hostName;
//
//	virtual bool regInfo() = 0;
//};

class rtspServer
{
public:
	static const rtspServer *getInstance()
	{
		return instance;
	}

private:
	static const rtspServer *instance;
	rtspServer(string file);

	//禁止拷贝构造以及赋值
	rtspServer(const rtspServer &);
	rtspServer & operator = (const rtspServer &);

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
	string fileName;
	sockaddr_in srvAddr;
	string hostName;

	void readFile();
};