/*--Author：李宏杰--*/

/*
	存放测试代码，并说明测试原理以及流程
	
	以后可以用来编写模块的Examples -> 这个想法不错啊！
*/

void testCode()
{
	//initServer();

	/*
		模块：日志记录模块
		文件：logger.h/.cpp

		测试思路：检查文件是否能正确处理，以及数据是否能够正确写入

		样例输入：10个运行数据，一个运行错误信息
		样例输出：正确的记录文件

		预期结果：不出错

		测试结果：成功
	*/
	{
		//logger *myLogger = logger::getInstance();

		//myLogger->initLogModule();

		//int i = 0;
		//while (i < 10)
		//{
		//	myLogger->logDelayData(i);
		//	++i;
		//}

		//myLogger->logError(123, "ADASDASDAS");
	}

	/*
		模块：中间件测试
		文件：middleWare.h/.cpp

		测试思路：其实就是正式代码了，没什么好测试的

		样例输入：无
		样例输出：无

		预期结果：不出错

		测试结果：成功
	*/
	{
		//middleWare *mWare = middleWare::getInstance();

		//mWare->startMiddleWare();

		//system("pause");
	}


	/*
		模块：流媒体信令解析模块与网络模块
		文件：rtspHandler.h/.cpp, cnctHandler.h/.cpp

		测试思路：
		使用流媒体模块随意编码一则信令[假定流媒体模块编码的信令为标准]，并通过网络模块发送，测试网络模块的发送功能
		这里需要一个测试服务端来显示收到的消息

		样例输入：标准流媒体信令
		样例输出：成功发送

		预期结果：成功发送

		测试结果：成功
	*/
	{
		//rtspHandler *rtsp = rtspHandler::getInstance();

		//cnctHandler *network = cnctHandler::getInstance();

		//network->connectServer();

		//string msg = rtsp->encodeMsg(OPTIONS);

		//network->sendMessage(msg);
		
		//system("pause");
	}

	/*
		模块：RTP模块
		文件：rtpHandler.h/.cpp

		测试思路：编写输入图像数据与各种头部，经过RTP模块的打包后，打好的包应该遵循预定好的标准并且数据与输入相同


		样例输入：随意编写的图像数据数组和图像头
		样例输出：与输入相同的信息

		预期结果：输出与输入相同

		测试结果：成功
	*/
	{
		//rtpHandler *rtp = rtpHandler::getInstance();

		//string msg;

		//vector<unsigned char> a = { 201,202,203,204,205,206,207,208 };

		//msg.resize(24 + a.size());

		//auto b = (imgHead *)(msg.c_str() + 16);
		//b->channels = htons(3);
		//b->imgType = htons(15);
		//b->xAxis.cols = htons(720);
		//b->yAxis.rows = htons(576);

		//memcpy(&msg[24], &a[0], a.size());

		//rtp->unpackRTP(msg);

		//WaitForSingleObject(hsRTPOutput, INFINITE);

		//imgHead aa;
		//vector<unsigned char> bb;

		//rtp->getMedia(aa, bb);

		//system("pause");
	}

	/*
		模块：控制信令模块
		文件：ctrlMsgHandler.h/.cpp

		测试思路：编写输入按键，经过编码后，编码好的信令应该遵循预定好的标准并且数据与输入相同


		样例输入：任意字母，如A
		样例输出：标准信令包，其中信息均为A的信息

		预期结果：标准信令包，内容部分与输入相同

		测试结果：成功
	*/
	{
		//ctrlMsgHandler *msgHandler = ctrlMsgHandler::getInstance();

		//msgHandler->keyboardMsgEncode('A');

		//string msg;
		//
		//WaitForSingleObject(hsCtrlMsgOutput, INFINITE);

		//msgHandler->getEncodedMsg(msg);
		//
		//system("pause");
	}

	/*
		模块：流媒体信令模块
		文件：rtspHandler.h/.cpp

		测试思路：编码部分对每个信令都进行编码并输出，检查是否正确；解码部分对手写的输入进行解码，检查是否能够获取正确错误码


		样例输入：手写错误码200/111信令各一条
		样例输出：200/111

		预期结果：输出正确的信令，输出正确的错误码

		测试结果：成功
	*/
	{
		//rtspHandler *rtsp = rtspHandler::getInstance();

		//for (int i = 1; i < 12;++i)cout << rtsp->encodeMsg(i) << endl;

		//string msg = "RTSP/1.0 200 OK\r\nCSeq: 1\r\n\r\n";

		//cout << rtsp->decodeMsg(msg) << endl;

		//msg = "RTSP/1.0 111 DAMN\r\nCSeq: 1\r\n\r\n";

		//rtsp->decodeMsg(msg);

		//system("pause");
	}

	/*
		模块：播放器模块与图像缓存模块
		文件：cvPlayer.h/.cpp

		测试思路：利用摄像头获取图像，然后塞入缓存，取出缓存，播放。检查流程是否通畅


		样例输入：摄像头
		样例输出：摄像头画面

		预期结果：正常播放

		测试结果：成功
	*/
	{
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
	}
}