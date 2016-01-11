#include "myServer.h"
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



			

//================================= MAIN =================================//
int main(int argc, char *argv[])
{	
	/*------------------------------建立连接--------------------------*/
	//初始化Winsock，版本2.2
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//获得单例
	cnctHandler *mySrv = cnctHandler::getInstance();










	return 0;
}

