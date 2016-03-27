/*--Author：李宏杰--*/

#include "myServer.h"
#include "cnctHandler.h"
#include "camCap.h"
#include "middleWare.h"

using namespace myNS;

/*
	专门输出前置提示信息
*/
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     交互式流媒体序列帧传输平台，版本v1.0     =" << endl;
	cout << "=            143020085211001 李宏杰            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--初始化中..." << endl;
	cout << "--监听连接..." << endl;
};
const myMessage myMsg;

/*
	初始化服务器
*/
void initServer()
{

	heShutdownSrv = CreateEvent(NULL, TRUE, FALSE, NULL);

	hsRenderInput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));
	hsRenderOutput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

	//hsImgBufferInput= CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::imgBufferInput));
	hsImgBufferOutput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::imgBufferOutput));

	hsCtrlMsgDecoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::ctrlMsgDecoded));
	hsCtrlMsgEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::ctrlMsgEncoded));

	hsRTPEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::rtpEncoded));

	hsMsgArrivedRTSP = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::msgArrivedRTSP));
	hsMsgArrivedCtrl = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::msgArrivedCtrl));
}

//================================= MAIN =================================//
int main()
{
	initServer();

	cnctHandler *netModule = cnctHandler::getInstance();

	netModule->startServer();

	middleWare *midWare = middleWare::getInstance();

	midWare->startMiddleWare();

	camCap *camera = camCap::getInstance();

	camera->startCapture();

	WaitForSingleObject(heShutdownSrv, INFINITE);

	/*
		剩余清理工作
	*/

	camera->stopCapture();

	//cnctHandler *srv = cnctHandler::getInstance();

	//srv->startServer();

	//WaitForSingleObject(hsMsgArrivedRTSP, INFINITE);

	//string msg;
	//SOCKET socket;

	//srv->getRTSPMsg(msg, socket);

	//srv->sendMessage(msg, socket);
	//WaitForSingleObject(hsMsgArrivedRTSP, INFINITE);
	//srv->getRTSPMsg(msg, socket);

	//srv->sendMessage(msg, socket);
	//WaitForSingleObject(heShutdownSrv, INFINITE);

	//rtpHandler *rtp = rtpHandler::getInstance();

	//vector<unsigned char> img = { 1,2,3,4,5,6,7,8 };

	//SOCKET socket = 123;

	//string msg;

	//rtp->pack(socket, img);

	//WaitForSingleObject(hsRTPEncoded, INFINITE);

	//rtp->getPacket(msg);

	//system("pause");

	//ctrlMsgHandler *ctrl = ctrlMsgHandler::getInstance();

	//string msg;

	//allMsgHead aHead;
	//aHead.msgSize = 0x00;
	//aHead.payloadType = KB_MSG;
	//aHead.cks = 0x0;
	//aHead.session = 0x12345678;

	//keyboardMsg kMsg;

	//kMsg.msgType = KB_ENG;
	//kMsg.isPressed = KB_CLICK;
	//kMsg.virtualCode = 0x41;
	//kMsg.unicode = 0x0041;

	//int size = sizeof(aHead) + sizeof(kMsg);
	//msg.resize(size);

	//memcpy((char *)(msg.c_str()), &aHead, sizeof(aHead));
	//
	//memcpy((char *)(msg.c_str()) + sizeof(aHead), &kMsg, sizeof(kMsg));

	//ctrl->decodeMsg(msg);

	//WaitForSingleObject(hsCtrlMsgDecoded, INFINITE);

	//unsigned int session;
	//char key;

	//ctrl->getDecodedMsg(session, key);

	//cout << session << endl << key << endl;

	////

	//imgHead head;
	//head.channels = 3;
	//head.cols = 720;
	//head.height = 576;
	//head.imgType = 15;
	//
	//ctrl->encodeMsg(head, 30, stoul("12345678"));

	//WaitForSingleObject(hsCtrlMsgEncoded, INFINITE);

	//string output;
	//ctrl->getEncodedMsg(output);

	//system("pause");
	//rtspHandler *rtsp = rtspHandler::getInstance();

	//string input;

	//input = "OPTIONS * RTSP/1.0\r\nCSeq: 1\r\n";

	//string output = rtsp->msgCodec(1, input);

	//cout << output << endl;

	//input = "DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0\r\nCSeq: 312\r\nAccept : application / sdp, application / rtsl, application / mheg\r\n";

	//output = rtsp->msgCodec(1, input);

	//cout << output << endl;

	//input = "SETUP rtsp ://example.com/foo/bar/baz.rm RTSP/1.0\r\nCSeq : 302\r\nTransport : RTP / AVP; unicast; client_port = 4588 - 4589\r\n";

	//output = rtsp->msgCodec(1, input);

	//cout << output << endl;

	//input = "PLAY rtsp://audio.example.com/meeting.en RTSP/1.0\r\nCSeq: 835\r\nSession : 12345678\r\n";

	//output = rtsp->msgCodec(1, input);

	//cout << output << endl;

	//input = "TEARDOWN rtsp://example.com/fizzle/foo RTSP/1.0\r\nCSeq: 892\r\nSession : 12345678\r\n";

	//output = rtsp->msgCodec(1, input);

	//cout << output << endl;

	//system("pause");

	//camCap *camera = camCap::getInstance();

	//imgBuffer *buf = imgBuffer::getInstance();

	//camera->startCapture();

	//Mat frame;

	//imgHead head, headOutput;
	//vector<unsigned char> imgData, imgOutput;

	//camera->changeFrameRate(5);

	//while (1)
	//{
	//	camera->render(90);

	//	WaitForSingleObject(hsRenderOutput, INFINITE);

	//	frame = camera->getImage();

	//	head.channels = frame.channels();
	//	head.imgType = frame.type();
	//	head.rows = frame.rows;

	//	imgData = frame.reshape(1, 1);

	//	buf->pushBuffer(head, imgData);

	//	WaitForSingleObject(hsImgBufferOutput, INFINITE);

	//	buf->popBuffer(headOutput, imgOutput);

	//	Mat cvFrame = Mat(imgOutput).reshape(headOutput.channels, headOutput.rows);

	//	cvFrame.convertTo(cvFrame, headOutput.imgType);

	//	imshow("Test", cvFrame);

	//	waitKey(5);
	//}

	return 0;
}