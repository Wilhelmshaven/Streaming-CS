/*--Author：李宏杰--*/

#include "myClient.h"

//加载中间件
#include "middleWare.h"

namespace myHandle
{
	HANDLE heCloseClient;

	HANDLE heESCPressed;
}
using namespace myHandle;

void initServer()
{
	//这里还要仔细考虑，现在更多的是一次性程序的感觉。
	//TODO：增加复位功能

	heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::clientClose);

	heESCPressed = CreateEvent(NULL, TRUE, FALSE, syncManager::ESCPressed);
}

// 专门输出前置提示信息
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     交互式流媒体序列帧传输平台，版本v1.0     =" << endl;
	cout << "=            143020085211001 李宏杰            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--初始化中..." << endl;
	cout << "--读入配置文件..." << endl;
};
const myMessage myMsg;

//================================= MAIN =================================//
int main(int argc, char *argv[])
{
	initServer();

	middleWare *mWare = middleWare::getInstance();

	mWare->startMiddleWare();

	WaitForSingleObject(heESCPressed, INFINITE);

	/*
		开始清理工作
	*/

	mWare->shutdownAll();

	return 0;
}