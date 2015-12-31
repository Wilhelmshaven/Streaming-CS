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


UINT rtspHandleThread();              //rtsp交互线程

HANDLE hCloseClient = CreateEvent(NULL, TRUE, FALSE, NULL);      //事件：关闭客户端，初值为NULL
HANDLE sendRecvMutex;             //rtsp收发互斥信号量

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
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)rtspHandleThread, NULL, NULL, NULL);          //rtsp交互线程
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
UINT rtspHandleThread()
{
	//收发与心跳子线程
	UINT sendMsgThread();
	UINT recvMsgThread();
	UINT rtspHeartBeat();

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)sendMsgThread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)recvMsgThread, NULL, NULL, NULL);

	//这个放在Play以后创建
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)rtspHeartBeat, NULL, NULL, NULL);

	//获取单例
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	//并用服务器信息设置rtsp处理器
	rtsp->setHandler(mySrv->getDisplayAddr(), MAKEWORD(1, 0), mySrv->getStreamPort(), true);

	string sendBuf, recvBuf;        //收发缓存
	int errCode;                    //错误代码
	int bytesSent;
	recvBuf.resize(BUF_SIZE);

	//考虑到阻塞和超时，回头在这里再开两个子线程，以便更好地控制。
	//首先发送DESCRIBE信令
	sendBuf = rtsp->encodeMsg(DESCRIBE);
	bytesSent = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.length(), NULL);

	//接收答复（求写成非阻塞）
	recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
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
	recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
	errCode = rtsp->decodeMsg(recvBuf);
	if (errCode != 200)
	{
		cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
		SetEvent(hCloseClient);   //设置一个结束事件，结束，看是结束所有还是什么
	}

	//然后是Play指令
	sendBuf = rtsp->encodeMsg(PLAY);
	send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

	//接收答复（求写成非阻塞）
	recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
	errCode = rtsp->decodeMsg(recvBuf);
	if (errCode != 200)
	{
		cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
		SetEvent(hCloseClient);   //设置一个结束事件，结束，看是结束所有还是什么
	}

	//正常的话，这里是心跳线程

	return 0;
}

UINT sendMsgThread()
{
	//收发可以使用互斥信号量，并设置超时
	sendRecvMutex = CreateMutex(NULL, TRUE, NULL);

	return 0;
}

UINT recvMsgThread()
{
	return 0;
}

UINT rtspHeartBeat()
{
	//等待启动信号


	//获取单例
	Server *mySrv = Server::getInstance();
	rtspHandler *rtsp = rtspHandler::getInstance();

	//计时器相关
	HANDLE heartBeatTimer = CreateWaitableTimer(NULL, FALSE, NULL);     //创建计时器，自动周期
	LARGE_INTEGER timeCnt;
	timeCnt.QuadPart = 0;                                               //0s后启动计时器（值负数）
	SetWaitableTimer(heartBeatTimer, &timeCnt, 30000, NULL, NULL, 0);   //第三个数为周期，单位毫秒.一般的流媒体系统60s超时
	
	string sendBuf;        //收发缓存
	int errCode;           //错误代码

	while (1)
	{
		if (WaitForSingleObject(hCloseClient, 0))break;              //检查是否需要结束	                  
		if (!WaitForSingleObject(heartBeatTimer, 10000))continue;    //等待10秒

		sendBuf = rtsp->encodeMsg(GET_PARAMETER);
		errCode = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);

		if (errCode != 200)
		{
			//如果出错
			cout << "心跳发送出错，错误代码：" << errCode << " " << rtsp->getErrMsg(errCode) << endl;
		}

		//不用收取回复了
	}

	return 0;
}