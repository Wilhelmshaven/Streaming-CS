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

UINT rtspControl(LPVOID lpParam);




//================================= MAIN =================================//
int main(int argc, char *argv[])
{
	/*------------------------------建立连接--------------------------*/
	//初始化Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建Socket;
	SOCKET streamingSocket = socket(AF_INET, SOCK_STREAM, 0);

	//获得服务器实例
	Server *mySrv = Server::getInstance();

	//Connect to server
	if (mySrv->connectServer() == 0)
	{
		streamingSocket = mySrv->getSocket();
	}
	else
	{
		//连接服务器失败
		system("pause");
		return 0;
	}

	/*------------------------------RTSP交互--------------------------*/


	system("pause");
	return 0;
}

//RTSP控制线程
UINT rtspControl(LPVOID lpParam)
{

	return 0;
}