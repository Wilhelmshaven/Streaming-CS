/*--Author������--*/

/*
	��Ų��Դ��룬��˵������ԭ���Լ�����
	
	�Ժ����������дģ���Examples -> ����뷨������
*/

void testCode()
{
	//initServer();

	/*
		ģ�飺��־��¼ģ��
		�ļ���logger.h/.cpp

		����˼·������ļ��Ƿ�����ȷ�����Լ������Ƿ��ܹ���ȷд��

		�������룺10���������ݣ�һ�����д�����Ϣ
		�����������ȷ�ļ�¼�ļ�

		Ԥ�ڽ����������

		���Խ�����ɹ�
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
		ģ�飺�м������
		�ļ���middleWare.h/.cpp

		����˼·����ʵ������ʽ�����ˣ�ûʲô�ò��Ե�

		�������룺��
		�����������

		Ԥ�ڽ����������

		���Խ�����ɹ�
	*/
	{
		//middleWare *mWare = middleWare::getInstance();

		//mWare->startMiddleWare();

		//system("pause");
	}


	/*
		ģ�飺��ý���������ģ��������ģ��
		�ļ���rtspHandler.h/.cpp, cnctHandler.h/.cpp

		����˼·��
		ʹ����ý��ģ���������һ������[�ٶ���ý��ģ����������Ϊ��׼]����ͨ������ģ�鷢�ͣ���������ģ��ķ��͹���
		������Ҫһ�����Է��������ʾ�յ�����Ϣ

		�������룺��׼��ý������
		����������ɹ�����

		Ԥ�ڽ�����ɹ�����

		���Խ�����ɹ�
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
		ģ�飺RTPģ��
		�ļ���rtpHandler.h/.cpp

		����˼·����д����ͼ�����������ͷ��������RTPģ��Ĵ���󣬴�õİ�Ӧ����ѭԤ���õı�׼����������������ͬ


		�������룺�����д��ͼ�����������ͼ��ͷ
		�����������������ͬ����Ϣ

		Ԥ�ڽ���������������ͬ

		���Խ�����ɹ�
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
		ģ�飺��������ģ��
		�ļ���ctrlMsgHandler.h/.cpp

		����˼·����д���밴������������󣬱���õ�����Ӧ����ѭԤ���õı�׼����������������ͬ


		�������룺������ĸ����A
		�����������׼�������������Ϣ��ΪA����Ϣ

		Ԥ�ڽ������׼����������ݲ�����������ͬ

		���Խ�����ɹ�
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
		ģ�飺��ý������ģ��
		�ļ���rtspHandler.h/.cpp

		����˼·�����벿�ֶ�ÿ��������б��벢���������Ƿ���ȷ�����벿�ֶ���д��������н��룬����Ƿ��ܹ���ȡ��ȷ������


		�������룺��д������200/111�����һ��
		���������200/111

		Ԥ�ڽ���������ȷ����������ȷ�Ĵ�����

		���Խ�����ɹ�
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
		ģ�飺������ģ����ͼ�񻺴�ģ��
		�ļ���cvPlayer.h/.cpp

		����˼·����������ͷ��ȡͼ��Ȼ�����뻺�棬ȡ�����棬���š���������Ƿ�ͨ��


		�������룺����ͷ
		�������������ͷ����

		Ԥ�ڽ������������

		���Խ�����ɹ�
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