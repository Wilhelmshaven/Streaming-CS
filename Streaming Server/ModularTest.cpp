/*--Author：李宏杰--*/

/*
	存放测试代码，并说明测试原理以及流程

	以后可以用来编写模块的Examples -> 这个想法不错啊！
*/

void testCode()
{
	/*
		模块：图像采集模块（摄像头）与图像缓存模块
		文件：camCap.h/.cpp, imageBuffer.h/.cpp

		测试思路：完成一个从图像采集到图像播放的本地完整数据流，验证模块功能与信号灯运作情况

		样例输入：摄像头模块显示的画面（源画面）
		样例输出：测试代码最终显示的画面

		预期结果：输入输出完全相同并完全同步（察觉不出延迟）

		测试结果：成功
	*/
	{
		//camCap *camera = camCap::getInstance();

		//imgBuffer *buf = imgBuffer::getInstance();

		////测试图像采集模块：开始采集功能
		//camera->startCapture();

		//Mat frame;

		//imgHead head, headOutput;
		//vector<unsigned char> imgData, imgOutput;

		////测试图像采集模块：设置帧率功能
		//camera->changeFrameRate(5);

		//while (1)
		//{
		//	//测试图像采集模块：传入控制信息并渲染的功能
		//	camera->render(90);

		//	//测试图像采集模块：输出信号灯（渲染完成）
		//	WaitForSingleObject(hsRenderOutput, INFINITE);

		//	//测试图像采集模块：取出图像功能
		//	frame = camera->getImage();

		//	//中间件部分代码
		//	{
		//		head.channels = frame.channels();
		//		head.imgType = frame.type();
		//		head.rows = frame.rows;

		//		imgData = frame.reshape(1, 1);
		//	}

		//	//测试图像缓存模块：推入缓存功能
		//	buf->pushBuffer(head, imgData);

		//	//测试图像缓存模块：输出信号灯（标记着缓存内有数据）
		//	WaitForSingleObject(hsImgBufferOutput, INFINITE);

		//	//测试图像缓存模块：取出缓存数据功能
		//	buf->popBuffer(headOutput, imgOutput);

		//	//模拟客户端播放器
		//	{
		//		Mat cvFrame = Mat(imgOutput).reshape(headOutput.channels, headOutput.rows);

		//		cvFrame.convertTo(cvFrame, headOutput.imgType);

		//		imshow("Test", cvFrame);

		//		waitKey(5);
		//	}
		//}
	}

	/*
		模块：流媒体信令处理模块
		文件：rtspHandler.h/.cpp

		测试思路：完成一个完整的流媒体信令交互流程

		样例输入：自拟流媒体信令
		样例输出：流媒体答复信令

		预期结果：流媒体答复信令格式与内容完全正确

		测试结果：成功
	*/
	{
		//rtspHandler *rtsp = rtspHandler::getInstance();

		//string input;

		////测试OPTIONS信令
		//input = "OPTIONS * RTSP/1.0\r\nCSeq: 1\r\n";

		//string output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		////测试DESCRIBE信令
		//input = "DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0\r\nCSeq: 312\r\nAccept : application / sdp, application / rtsl, application / mheg\r\n";

		//output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		////测试SETUP信令
		//input = "SETUP rtsp ://example.com/foo/bar/baz.rm RTSP/1.0\r\nCSeq : 302\r\nTransport : RTP / AVP; unicast; client_port = 4588 - 4589\r\n";

		//output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		////测试PLAY信令
		//input = "PLAY rtsp://audio.example.com/meeting.en RTSP/1.0\r\nCSeq: 835\r\nSession : 12345678\r\n";

		//output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		////测试TEARDOWN信令
		//input = "TEARDOWN rtsp://example.com/fizzle/foo RTSP/1.0\r\nCSeq: 892\r\nSession : 12345678\r\n";

		//output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		//system("pause");
	}

	/*
		模块：控制信令处理模块
		文件：ctrlMsgHandler.h/.cpp

		测试思路：自拟数据包，测试编解码功能，看编解码之后的结果是否符合预期

		样例输入：自拟控制信令/数据包
		样例输出：数据包/信令

		预期结果：编解码后的内容与拟定的内容完全一致，字节序正确

		测试结果：成功
	*/
	{
		//ctrlMsgHandler *ctrl = ctrlMsgHandler::getInstance();

		////手动填一个测试数据包
		//{
		//	string msg;

		//	allMsgHead aHead;
		//	aHead.msgSize = 0x00;
		//	aHead.payloadType = KB_MSG;
		//	aHead.cks = 0x0;
		//	aHead.session = 0x12345678;

		//	keyboardMsg kMsg;

		//	kMsg.msgType = KB_ENG;
		//	kMsg.isPressed = KB_CLICK;
		//	kMsg.virtualCode = 0x41;
		//	kMsg.unicode = 0x0041;

		//	int size = sizeof(aHead) + sizeof(kMsg);
		//	msg.resize(size);

		//	memcpy((char *)(msg.c_str()), &aHead, sizeof(aHead));

		//	memcpy((char *)(msg.c_str()) + sizeof(aHead), &kMsg, sizeof(kMsg));
		//}

		////测试解码功能
		//ctrl->decodeMsg(msg);

		////测试解码信号灯
		//WaitForSingleObject(hsCtrlMsgDecoded, INFINITE);

		//unsigned int session;
		//char key;

		////测试取出解码后的信息的功能
		//ctrl->getDecodedMsg(session, key);

		//cout << session << endl << key << endl;

		////手编图像头数据
		//{
		//	imgHead head;
		//	head.channels = 3;
		//	head.cols = 720;
		//	head.height = 576;
		//	head.imgType = 15;
		//}
		//
		////测试编码图像头功能
		//ctrl->encodeMsg(head, 30, stoul("12345678"));

		////测试编码信号灯
		//WaitForSingleObject(hsCtrlMsgEncoded, INFINITE);

		//string output;
		//ctrl->getEncodedMsg(output);

		//system("pause");
	}

	/*
		模块：网络模块
		文件：cnctHandler.h/.cpp

		测试思路：利用模块接口实现回声服务器

		样例输入：使用两个客户端一起连接并任意发送（或不发送）
		样例输出：服务端报出收到的消息，客户端报出收到的消息

		预期结果：回声

		测试结果：成功
	*/
	{
		//cnctHandler *srv = cnctHandler::getInstance();

		////测试启动服务器功能
		//srv->startServer();

		////测试信号灯
		//WaitForSingleObject(hsMsgArrivedRTSP, INFINITE);

		//string msg;
		//SOCKET socket;

		////测试取出消息功能
		//srv->getRTSPMsg(msg, socket);

		////测试发送消息功能
		//srv->sendMessage(msg, socket);

		////重复一遍
		//WaitForSingleObject(hsMsgArrivedRTSP, INFINITE);
		//srv->getRTSPMsg(msg, socket);
		//srv->sendMessage(msg, socket);

		////用于挂起主线程
		//WaitForSingleObject(heShutdownSrv, INFINITE);
	}

	/*
		模块：RTP模块
		文件：rtpHandler.h/.cpp

		测试思路：自拟数据，观察打包结果

		样例输入：自拟数据
		样例输出：正确编码的RTP数据包

		预期结果：正确编码的RTP数据包

		测试结果：成功
	*/
	{
		//rtpHandler *rtp = rtpHandler::getInstance();

		//vector<unsigned char> img = { 1,2,3,4,5,6,7,8 };

		//SOCKET socket = 123;

		//string msg;

		////测试编码（打包）功能
		//rtp->pack(socket, img);

		////测试信号灯
		//WaitForSingleObject(hsRTPEncoded, INFINITE);

		////测试取出数据包功能
		//rtp->getPacket(msg);

		//system("pause");
	}
}