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
	//初始化Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建Socket;
	SOCKET streamingSocket = socket(AF_INET, SOCK_STREAM, 0);

	//获得服务器实例
	Server *mySrv = Server::getInstance();


	//--Test code--//
	//in_addr address;
	//sockaddr_in ServerAddr;

	//ServerAddr.sin_family = AF_INET;
	//inet_pton(AF_INET, "192.168.1.101", &address);
	//ServerAddr.sin_addr = address;
	//ServerAddr.sin_port = htons(atoi("554"));

	//int flag = connect(streamingSocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr));//这里是可能超时的，默认超时时间是75秒

	//if (flag == 0)mySrv->setSocket(streamingSocket);
	//--End of test code--//

	//Connect to server
	if (mySrv->connectServer() == 0)
	{
		streamingSocket = mySrv->getSocket();
	}
	else
	{
		//连接服务器失败
		return 0;
	}

	//RTSP交互
	rtspHandler rtspModule;
	serverList *srvInfo = mySrv->getServerInfo();
	rtspModule.initRTSPmsg(mySrv->getDisplayAddr(), atoi(srvInfo->getConfigByIndex(port).c_str()), streamingSocket);

	//创建线程
	rtspParam rtspPara;
	rtspPara.URI = mySrv->getDisplayAddr();
	rtspPara.Port = atoi(srvInfo->getConfigByIndex(port).c_str());
	rtspPara.socket = streamingSocket;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rtspControl, &rtspPara, 0, NULL);



	return 0;
}

//RTSP控制线程
UINT rtspControl(LPVOID lpParam)
{
	rtspParam *param = (rtspParam *)lpParam;
	rtspHandler rtspMsg;            // RTSP信令相关
	int flag;

	//发送RTSP信令，并对答令进行相应解析
	rtspMsg.initRTSPmsg(param->URI, param->Port, param->socket);     // 初始化
	flag = rtspMsg.play();                                           // 播放，即完成从OPTION到PLAY的信令发送工作
	if (flag != 200)
	{
		cout << "Error in playing media." << endl;
		return 0;
	}

	// 心跳
	int timer = 0;
	while (1)
	{
		timer++;           //计时器

		if (timer > 100000)
		{
			timer = 0;
			//rtspMsg.HeartBeat();
		}

		////事件控制结束
		//if (WaitForSingleObject(hEvent_Close, 1) == WAIT_OBJECT_0)
		//{
		//	break;
		//}
	}

	return 0;
}