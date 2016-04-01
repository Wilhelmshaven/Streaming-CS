#include "CommonHeaders.h"

#include "myClient.h"

#include "rtpHandler.h"

namespace myHandle
{
	HANDLE heCloseClient;

	HANDLE hsPlayerOutput;

	HANDLE hsBufferOutput;

	HANDLE hsRTPOutput;

	HANDLE hsCtrlMsgOutput;
};

using namespace myHandle;

void initServer()
{
	//这里还要仔细考虑，现在更多的是一次性程序的感觉。
	//TODO：增加复位功能

	heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::clientClose);

	hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

	hsBufferOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::bufferOutput);

	hsRTPOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::rtpOutput);

	hsCtrlMsgOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgOutput);
}

int main()
{
	initServer();

	//rtpHandler *rtp = rtpHandler::getInstance();

	//string msg;

	//vector<unsigned char> a = { 201,202,203,204,205,206,207,208 };

	//msg.resize(36 + a.size());

	//memcpy(&msg[36], &a[0], a.size());

	//rtp->unpackRTP(msg);

	//WaitForSingleObject(hsRTPOutput, INFINITE);

	//imgHead aa;
	//vector<unsigned char> bb;

	//rtp->getMedia(aa, bb);

	//system("pause");

	//ctrlMsgHandler *msgHandler = ctrlMsgHandler::getInstance();

	//msgHandler->keyboardMsgEncode('A');

	//string msg;
	//
	//WaitForSingleObject(hsCtrlMsgOutput, INFINITE);

	//msgHandler->getEncodedMsg(msg);
	//
	//system("pause");

	//rtspHandler *rtsp = rtspHandler::getInstance();

	//for (int i = 1; i < 12;++i)cout << rtsp->encodeMsg(i) << endl;

	//string msg = "RTSP/1.0 200 OK\r\nCSeq: 1\r\n\r\n";

	//cout << rtsp->decodeMsg(msg) << endl;

	//msg= "RTSP/1.0 111 DAMN\r\nCSeq: 1\r\n\r\n";

	//rtsp->decodeMsg(msg);

	//system("pause");

	//cvPlayer *camera = cvPlayer::getInstance();

	//imgBuffer *buffer = imgBuffer::getInstance();

	//VideoCapture capture(0);

	//Mat frame;

	//camera->play();

	//vector<char> img;

	//imgHead head;

	//while (1)
	//{
	//	capture >> frame;

	//	string s;

	//	img = frame.reshape(1, 1);

	//	head.xAxis.cols = frame.rows;
	//	head.imgType = frame.type();
	//	head.channels = frame.channels();

	//	buffer->pushBuffer(head, img);

	//	WaitForSingleObject(hsBufferOutput, INFINITE);

	//	buffer->popBuffer(head, img);

	//	camera->insertImage(head, img);

	//	waitKey(50);

	//	if (WaitForSingleObject(hsPlayerOutput, 0) == WAIT_OBJECT_0)
	//	{
	//		char key;

	//		camera->getCtrlKey(key);

	//		cout << key << endl;
	//	}
	//}

	return 0;
}