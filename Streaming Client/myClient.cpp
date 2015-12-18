#include "Server.h"
#include "myClient.h"
#include "rtspHandler.h"

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
	cout << "--读入配置文件..." << endl;
};
const myMessage myMsg;


UINT rtspInteract();                  //rtsp交互线程
UINT rtspHeartBeat();                 //rtsp心跳线程

HANDLE hCloseClient = CreateEvent(NULL, TRUE, FALSE, NULL);      //事件：关闭客户端，初值为NULL


//================================= MAIN =================================//
int main(int argc, char *argv[])
{
	/*------------------------------建立连接--------------------------*/
	//初始化Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//获得服务器实例
	Server *mySrv = Server::getInstance();

	//Connect to server
	if (mySrv->connectServer() == 0)
	{
		/*------------------------------RTSP交互--------------------------*/
		//这个必然是要放在子线程里了
		//所以首先新建两个线程。一个是控制交互，一个是心跳（作为交互的子线程就好），维持连接的
		//线程不需要参数了，单例里面要什么有什么
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)rtspInteract, NULL, NULL, NULL);          //rtsp交互线程
	}
	else
	{
		//连接服务器失败
	}

	WaitForSingleObject(hCloseClient, INFINITE);
	system("pause");
	return 0;
}

//RTSP控制线程
UINT rtspControl(LPVOID lpParam)
{
	//获取单例
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	string sendBuf, recvBuf;        //收发缓存
	int errCode;                    //错误代码

	//首先发送DESCRIBE信令
	sendBuf = rtsp->encodeMsg(DESCRIBE);
	send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

	//接收答复（求写成非阻塞）
	recv(mySrv->getSocket(), (char *)recvBuf.data(), BUF_SIZE, NULL);
	errCode = rtsp->decodeMsg(recvBuf);
	if (errCode != 200)
	{
		cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
		SetEvent(hCloseClient);   //设置一个结束事件，结束，看是结束所有还是什么
	}

	//然后发送Setup信令
	sendBuf = rtsp->encodeMsg(SETUP);
	send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

	//接收答复（求写成非阻塞）

	return 0;
}