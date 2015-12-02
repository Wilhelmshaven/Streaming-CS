#include "CommonHeaders.h"
#include "rtspServer.h"

int main()
{
	cout << "======交互式流媒体序列帧传输平台，版本v0.1======" << endl;
	cout << "--初始化中..." << endl;
	cout << "--读取服务器信息..." << endl;

	const rtspServer* myServer = rtspServer::getInstance();  //获取流媒体服务器实例

	return 0;
}