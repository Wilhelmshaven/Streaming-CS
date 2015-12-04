#include "CommonHeaders.h"
#include "rtspServer.h"
#include "myClient.h"

// 专门输出前置提示信息
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     交互式流媒体序列帧传输平台，版本v0.1     =" << endl;
	cout << "=            143020085211001 李宏杰            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--初始化中..." << endl;
	cout << "--读取服务器信息..." << endl;
};
const myMessage myMsg;

int main(int argc, char *argv[])
{

	rtspServer* myServer = rtspServer::getInstance();  //获取流媒体服务器实例


	return 0;
}