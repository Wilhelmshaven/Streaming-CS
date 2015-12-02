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

	//��ֹ���������Լ���ֵ
	rtspServer(const rtspServer &);
	rtspServer & operator = (const rtspServer &);

	//��������
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