/*--Author：李宏杰--*/

#include "myServer.h"

//加载中间件
#include "middleWare.h"

/*
	专门输出前置提示信息
*/
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     交互式流媒体序列帧传输平台，版本v1.0     =" << endl;
	cout << "=            143020085211001 李宏杰            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--初始化中..." << endl;
	cout << "--监听连接..." << endl;
};
const myMessage myMsg;

namespace myServerNS
{
	//全局事件：结束服务器的事件
	HANDLE heSrvDown;
}
using namespace myServerNS;

/*
	初始化服务器
*/
void initServer()
{
	heSrvDown = CreateEvent(NULL, TRUE, FALSE, TEXT(syncManager::srvDown));
}

//================================= MAIN =================================//
int main()
{
	initServer();

	middleWare *midWare = middleWare::getInstance();

	midWare->startMiddleWare();

	WaitForSingleObject(heSrvDown, INFINITE);

	/*
		剩余清理工作
	*/

	return 0;
}