#include "CommonHeaders.h"

//设置一个父类应该还是有必要的
//Server类不涉及具体操作，也就是保存一些连接信息而已
class Server
{
protected:
	sockaddr_in srvAddr;
	string hostName;

	
};