/*--Author：李宏杰--*/

#include "myClient.h"

//加载中间件
#include "middleWare.h"

//加载网络处理模块
#include "cnctHandler.h"

//加载播放器模块
#include "cvPlayer.h"

//加载RTSP模块
#include "rtspHandler.h"

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

void sendMsgBackground(int msgType);

//================================= MAIN =================================//
int main(int argc, char *argv[])
{
	/*------------------------------建立连接--------------------------*/
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	cnctHandler *mySrv = cnctHandler::getInstance();

	if (mySrv->connectServer() == 0)
	{
		//成功连接服务器

		/*
			启动中间件
		*/

		mwPlayCtrl *middleWare = mwPlayCtrl::getInstance();

		middleWare->startMiddleWare();

		/*
			RTSP交互
			算了不写在RTSP模块里了，就直接主函数吧
			反正客户端简单
		*/

		rtspHandler *rtspModule = rtspHandler::getInstance();

		string rtspMsg;

		int errCode;

		//1.发送OPTIONS信令
		sendMsgBackground(OPTIONS);

		if (WaitForSingleObject(hsIsRTSPOK, 5000) != WAIT_OBJECT_0)
		{
			SetEvent(hCloseClientEvent);

			goto endClient;
		}
		        
		//2.发送DESCRIBE信令
		sendMsgBackground(DESCRIBE);

		if (WaitForSingleObject(hsIsRTSPOK, 5000) != WAIT_OBJECT_0)
		{
			SetEvent(hCloseClientEvent);

			goto endClient;
		}

		//3.发送SETUP信令
		sendMsgBackground(SETUP);

		if (WaitForSingleObject(hsIsRTSPOK, 5000) != WAIT_OBJECT_0)
		{
			SetEvent(hCloseClientEvent);

			goto endClient;
		}

		//4.发送PLAY信令
		sendMsgBackground(PLAY);

		if (WaitForSingleObject(hsIsRTSPOK, 5000) != WAIT_OBJECT_0)
		{
			SetEvent(hCloseClientEvent);

			goto endClient;
		}

		//开始心跳
		SetEvent(heStartHeartBeat);
	}
	else
	{
		//连接服务器失败
		goto endClient;
	}

	WaitForSingleObject(hCloseClientEvent, INFINITE);

	/*
		结束清理工作
	*/

	//5.发送TEARDOWN信令
	sendMsgBackground(TEARDOWN);

endClient:

	ResetEvent(heStartHeartBeat);

	CloseHandle(hCloseClientEvent);

	system("pause");

	return 0;
}

/*
	发送RTSP信令的函数（一个小封装，编码->发送）
*/
void sendMsgBackground(int msgType)
{
	cnctHandler *networkModule = cnctHandler::getInstance();

	rtspHandler *rtspModule = rtspHandler::getInstance();

	string rtspMsg;

	rtspMsg = rtspModule->encodeMsg(msgType);

	networkModule->sendMessage(rtspMsg);
}





////RTSP信令发送线程
//void sendMsgInThread(int msgType); //发送信令线程内的函数（一个小封装，编码->发送->错误处理）
//UINT sendMsgThread()
//{
//	//获取单例
//	cnctHandler *mySrv = cnctHandler::getInstance();
//	rtspHandler *rtsp = rtspHandler::getInstance();
//
//	sendRecvMutex = CreateMutex(NULL, TRUE, NULL);          //收发可以使用互斥信号量，并设置超时
//	WaitForSingleObject(hRTSPBeginEvent,INFINITE);          //等待开始指令    
//
//	string sendBuf, recvBuf;        //收发缓存
//	int bytesSent;                  //发送的字节数
//
//	sendMsgInThread(DESCRIBE);      //首先发送DESCRIBE信令
//
//	//互斥量
//	ReleaseMutex(sendRecvMutex);
//	Sleep(1000);                    //只是为了方便测试，下同
//	WaitForSingleObject(sendRecvMutex, INFINITE);
//
//	sendMsgInThread(SETUP);             //然后发送Setup信令
//
//	//互斥量
//	ReleaseMutex(sendRecvMutex);
//	Sleep(1000);
//	WaitForSingleObject(sendRecvMutex, INFINITE);
//
//	sendMsgInThread(PLAY);                                   //然后是Play指令
//	hBeatStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);  //创建心跳事件
//
//	//互斥量
//	ReleaseMutex(sendRecvMutex);
//	Sleep(1000);
//	WaitForSingleObject(sendRecvMutex, INFINITE);
//
//	//等待结束指令，挂起线程（不需要再发什么了）
//	WaitForSingleObject(hCloseClientEvent, INFINITE);
//	ReleaseMutex(sendRecvMutex);
//
//	sendMsgInThread(TEARDOWN);//TEARDOWN，断开连接
//
//	return 0;
//}
//

//
////RTSP信令接收线程
//UINT recvMsgThread()
//{
//	//获取单例
//	cnctHandler *mySrv = cnctHandler::getInstance();
//	rtspHandler *rtsp = rtspHandler::getInstance();
//
//	string recvBuf;        //收发缓存
//	int errCode;           //错误代码
//	
//	WaitForSingleObject(hRTSPBeginEvent, INFINITE);         //等待线程开始事件
//
//	while (1)
//	{
//		WaitForSingleObject(sendRecvMutex, INFINITE);       //等待收发互斥量（一收一发）
//		WaitForSingleObject(hCloseClientEvent, 0);          //检查结束事件是否已被设置
//
//		recvBuf.clear();
//		recvBuf.resize(BUF_SIZE);
//
//		//接收答复（目前通过设置超时避免阻塞过久）
//		recv(mySrv->getSocket(), (char *)recvBuf.data(), recvBuf.length(), NULL);
//		recvBuf = recvBuf.substr(0, recvBuf.find('\0'));         //紧缩长度
//		errCode = rtsp->decodeMsg(recvBuf);
//		if (errCode != 200)
//		{
//			cout << "Error:" << rtsp->getErrMsg(errCode) << endl;
//			SetEvent(hCloseClientEvent);   //设置一个结束事件，结束，看是结束所有还是什么
//			ReleaseMutex(sendRecvMutex);
//			break;
//		}
//		else
//		{
//			cout << "Msg recv: " << endl << recvBuf << endl;
//
//			//如果心跳事件被创建（发送了PLAY），这里OK，就设置启动
//			if (hBeatStartEvent != NULL)SetEvent(hBeatStartEvent);   
//		}
//
//		ReleaseMutex(sendRecvMutex);
//		Sleep(1000);               //便于测试
//	}
//
//	return 0;
//}
//
////RTSP心跳线程，30秒发送一次GET_PARAMETER信令直到结束客户端事件被设置
////已完成
//UINT rtspHeartBeat()
//{
//	//等待启动信号
//	WaitForSingleObject(hBeatStartEvent, INFINITE);
//
//	//获取单例
//	cnctHandler *mySrv = cnctHandler::getInstance();
//	rtspHandler *rtsp = rtspHandler::getInstance();
//
//	//计时器相关
//	HANDLE heartBeatTimer = CreateWaitableTimer(NULL, FALSE, NULL);     //创建计时器，自动周期
//	LARGE_INTEGER timeCnt;
//	timeCnt.QuadPart = 0;                                               //0s后启动计时器（值负数）
//	SetWaitableTimer(heartBeatTimer, &timeCnt, 30000, NULL, NULL, 0);   //第三个数为周期，单位毫秒.一般的流媒体系统60s超时
//	
//	string sendBuf;        //收发缓存
//	int errCode;           //错误代码
//
//	while (1)
//	{
//		if (WaitForSingleObject(hCloseClientEvent, 0))break;     //检查是否需要结束	 
//
//		if (WaitForSingleObject(heartBeatTimer, 1000))           //每秒检查一次
//		{
//			sendBuf = rtsp->encodeMsg(GET_PARAMETER);
//			errCode = send(mySrv->getSocket(), sendBuf.c_str(), sendBuf.size(), NULL);
//
//			if (errCode != 200)
//			{
//				//如果出错
//				cout << "心跳发送出错，错误代码：" << errCode << " " << rtsp->getErrMsg(errCode) << endl;
//			}
//
//			//不用收取回复了
//		}	
//	}
//
//	return 0;
//}