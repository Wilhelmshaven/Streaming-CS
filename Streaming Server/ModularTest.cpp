/*--Author������--*/

/*
	��Ų��Դ��룬��˵������ԭ���Լ�����

	�Ժ����������дģ���Examples -> ����뷨������
*/

void testCode()
{
	/*
		ģ�飺ͼ��ɼ�ģ�飨����ͷ����ͼ�񻺴�ģ��
		�ļ���camCap.h/.cpp, imageBuffer.h/.cpp

		����˼·�����һ����ͼ��ɼ���ͼ�񲥷ŵı�����������������֤ģ�鹦�����źŵ��������

		�������룺����ͷģ����ʾ�Ļ��棨Դ���棩
		������������Դ���������ʾ�Ļ���

		Ԥ�ڽ�������������ȫ��ͬ����ȫͬ������������ӳ٣�

		���Խ�����ɹ�
	*/
	{
		//camCap *camera = camCap::getInstance();

		//imgBuffer *buf = imgBuffer::getInstance();

		////����ͼ��ɼ�ģ�飺��ʼ�ɼ�����
		//camera->startCapture();

		//Mat frame;

		//imgHead head, headOutput;
		//vector<unsigned char> imgData, imgOutput;

		////����ͼ��ɼ�ģ�飺����֡�ʹ���
		//camera->changeFrameRate(5);

		//while (1)
		//{
		//	//����ͼ��ɼ�ģ�飺���������Ϣ����Ⱦ�Ĺ���
		//	camera->render(90);

		//	//����ͼ��ɼ�ģ�飺����źŵƣ���Ⱦ��ɣ�
		//	WaitForSingleObject(hsRenderOutput, INFINITE);

		//	//����ͼ��ɼ�ģ�飺ȡ��ͼ����
		//	frame = camera->getImage();

		//	//�м�����ִ���
		//	{
		//		head.channels = frame.channels();
		//		head.imgType = frame.type();
		//		head.rows = frame.rows;

		//		imgData = frame.reshape(1, 1);
		//	}

		//	//����ͼ�񻺴�ģ�飺���뻺�湦��
		//	buf->pushBuffer(head, imgData);

		//	//����ͼ�񻺴�ģ�飺����źŵƣ�����Ż����������ݣ�
		//	WaitForSingleObject(hsImgBufferOutput, INFINITE);

		//	//����ͼ�񻺴�ģ�飺ȡ���������ݹ���
		//	buf->popBuffer(headOutput, imgOutput);

		//	//ģ��ͻ��˲�����
		//	{
		//		Mat cvFrame = Mat(imgOutput).reshape(headOutput.channels, headOutput.rows);

		//		cvFrame.convertTo(cvFrame, headOutput.imgType);

		//		imshow("Test", cvFrame);

		//		waitKey(5);
		//	}
		//}
	}

	/*
		ģ�飺��ý�������ģ��
		�ļ���rtspHandler.h/.cpp

		����˼·�����һ����������ý�����������

		�������룺������ý������
		�����������ý�������

		Ԥ�ڽ������ý��������ʽ��������ȫ��ȷ

		���Խ�����ɹ�
	*/
	{
		//rtspHandler *rtsp = rtspHandler::getInstance();

		//string input;

		////����OPTIONS����
		//input = "OPTIONS * RTSP/1.0\r\nCSeq: 1\r\n";

		//string output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		////����DESCRIBE����
		//input = "DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0\r\nCSeq: 312\r\nAccept : application / sdp, application / rtsl, application / mheg\r\n";

		//output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		////����SETUP����
		//input = "SETUP rtsp ://example.com/foo/bar/baz.rm RTSP/1.0\r\nCSeq : 302\r\nTransport : RTP / AVP; unicast; client_port = 4588 - 4589\r\n";

		//output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		////����PLAY����
		//input = "PLAY rtsp://audio.example.com/meeting.en RTSP/1.0\r\nCSeq: 835\r\nSession : 12345678\r\n";

		//output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		////����TEARDOWN����
		//input = "TEARDOWN rtsp://example.com/fizzle/foo RTSP/1.0\r\nCSeq: 892\r\nSession : 12345678\r\n";

		//output = rtsp->msgCodec(1, input);

		//cout << output << endl;

		//system("pause");
	}

	/*
		ģ�飺���������ģ��
		�ļ���ctrlMsgHandler.h/.cpp

		����˼·���������ݰ������Ա���빦�ܣ��������֮��Ľ���Ƿ����Ԥ��

		�������룺�����������/���ݰ�
		������������ݰ�/����

		Ԥ�ڽ������������������ⶨ��������ȫһ�£��ֽ�����ȷ

		���Խ�����ɹ�
	*/
	{
		//ctrlMsgHandler *ctrl = ctrlMsgHandler::getInstance();

		////�ֶ���һ���������ݰ�
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

		////���Խ��빦��
		//ctrl->decodeMsg(msg);

		////���Խ����źŵ�
		//WaitForSingleObject(hsCtrlMsgDecoded, INFINITE);

		//unsigned int session;
		//char key;

		////����ȡ����������Ϣ�Ĺ���
		//ctrl->getDecodedMsg(session, key);

		//cout << session << endl << key << endl;

		////�ֱ�ͼ��ͷ����
		//{
		//	imgHead head;
		//	head.channels = 3;
		//	head.cols = 720;
		//	head.height = 576;
		//	head.imgType = 15;
		//}
		//
		////���Ա���ͼ��ͷ����
		//ctrl->encodeMsg(head, 30, stoul("12345678"));

		////���Ա����źŵ�
		//WaitForSingleObject(hsCtrlMsgEncoded, INFINITE);

		//string output;
		//ctrl->getEncodedMsg(output);

		//system("pause");
	}

	/*
		ģ�飺����ģ��
		�ļ���cnctHandler.h/.cpp

		����˼·������ģ��ӿ�ʵ�ֻ���������

		�������룺ʹ�������ͻ���һ�����Ӳ����ⷢ�ͣ��򲻷��ͣ�
		�������������˱����յ�����Ϣ���ͻ��˱����յ�����Ϣ

		Ԥ�ڽ��������

		���Խ�����ɹ�
	*/
	{
		//cnctHandler *srv = cnctHandler::getInstance();

		////������������������
		//srv->startServer();

		////�����źŵ�
		//WaitForSingleObject(hsMsgArrivedRTSP, INFINITE);

		//string msg;
		//SOCKET socket;

		////����ȡ����Ϣ����
		//srv->getRTSPMsg(msg, socket);

		////���Է�����Ϣ����
		//srv->sendMessage(msg, socket);

		////�ظ�һ��
		//WaitForSingleObject(hsMsgArrivedRTSP, INFINITE);
		//srv->getRTSPMsg(msg, socket);
		//srv->sendMessage(msg, socket);

		////���ڹ������߳�
		//WaitForSingleObject(heShutdownSrv, INFINITE);
	}

	/*
		ģ�飺RTPģ��
		�ļ���rtpHandler.h/.cpp

		����˼·���������ݣ��۲������

		�������룺��������
		�����������ȷ�����RTP���ݰ�

		Ԥ�ڽ������ȷ�����RTP���ݰ�

		���Խ�����ɹ�
	*/
	{
		//rtpHandler *rtp = rtpHandler::getInstance();

		//vector<unsigned char> img = { 1,2,3,4,5,6,7,8 };

		//SOCKET socket = 123;

		//string msg;

		////���Ա��루���������
		//rtp->pack(socket, img);

		////�����źŵ�
		//WaitForSingleObject(hsRTPEncoded, INFINITE);

		////����ȡ�����ݰ�����
		//rtp->getPacket(msg);

		//system("pause");
	}
}