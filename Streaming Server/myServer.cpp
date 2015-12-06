#include "myServer.h"
#include "CompletionPort.h"

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

UINT ListeningThread(LPVOID lpParam);                                                   //监听线程
HANDLE workThread;                              // 线程句柄

int main(int argc, char *argv[])
{
	//初始化Winsock，版本2.2
	workThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListeningThread, NULL, 0, NULL);


	return 0;
}

//监听线程
UINT ListeningThread(LPVOID lpParam)
{
	Comport cPort;                                  // Winsock完成端口对象
	//cPort.SetPort(sp.port);
	cPort.BindSocket();
	cPort.StartListen();

	return 0;
}