#include "myServer.h"

/////加载各个模块/////

//1.摄像头采集模块：采集图像
#include "camCap.h"

//2.图像队列模块（包括图片缓存）：把图像装入图像队列
#include "imageQueue.h"

//3.连接处理模块（Winsock）：发出图像，收发信令
#include "cnctHandler.h"

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
	cout << "--监听连接..." << endl;
};
const myMessage myMsg;


UINT sendImgQueue();                   //发送图像线程？

//================================= MAIN =================================//
int main(int argc, char *argv[])
{	
	/*------------------------------建立连接--------------------------*/
	//初始化Winsock，版本2.2
	//WSADATA wsaData;
	//WSAStartup(MAKEWORD(2, 2), &wsaData);

	////获得单例
	//cnctHandler *mySrv = cnctHandler::getInstance();

	//mySrv->startServer();

	camCap test;

	test.startCapture(20);



	WaitForSingleObject(hSrvShutdown, INFINITE);

	return 0;
}

UINT sendImgQueue()
{
	return 0;
}

