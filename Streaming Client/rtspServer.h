#include "CommonHeaders.h"

//继承自Server？
class rtspServer
{
public:
	static rtspServer *getInstance()
	{
		return instance;
	}

	void test();

private:
	static rtspServer *instance;
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
	string fileName;           //配置文件名
	sockaddr_in srvAddr;       //地址结构（ipv4/6，IP，端口）
	string displayAddr;        //完整的播放地址，eg. http://www.rayion.com/desktop

	bool readFile();  // 读取配置文件
};