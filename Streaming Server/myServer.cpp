/*--Author：李宏杰--*/

#include "myServer.h"

/*
	加载各个模块
*/

//加载网络模块，启动之
#include "cnctHandler.h"

//加载中间件，启动之
#include "middleWare.h"

//加载摄像头，启动之
#include "camCap.h"

/*
	专门输出前置提示信息
*/
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     交互式流媒体序列帧传输平台，版本v0.1     =" << endl;
	cout << "=            143020085211001 李宏杰            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--初始化中..." << endl;
	cout << "--监听连接..." << endl;
};
const myMessage myMsg;

//================================= MAIN =================================//
int main(int argc, char *argv[])
{	
	/*------------------------------建立连接--------------------------*/
	
	//启动网络模块
	cnctHandler *networkModule = cnctHandler::getInstance();
	networkModule->startServer();

	//启动中间件
	mwMsg *middleWare = mwMsg::getInstance();
	middleWare->startMiddleWare();

	//启动摄像头
	camCap *camera = camCap::getInstance();
	camera->startCapture();

	//TODO：启动……

	


	WaitForSingleObject(hSrvShutdown, INFINITE);

	//TODO：一系列善后处理
	//delete networkModule;
	//delete middleWare;
	//delete camera;

	return 0;
}