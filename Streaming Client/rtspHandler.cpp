/*--Author������--*/

#include "rtspHandler.h"

#include "errHandler.h"

/*----------------------RTSP���Ӵ�����---------------------*/
rtspHandler *rtspHandler::instance = new rtspHandler();

rtspHandler* rtspHandler::getInstance()
{
	return instance;
}

/*
	RTSP���������캯��

	Ĭ��ֵ��
	���кţ�Sequence number����1
*/
rtspHandler::rtspHandler()
{
	//��ʼ�����к�Ϊ1
	seqNum = 1;          
	
	rtspVersion = "1.0";

	streamingPort = 8554;

	enableUDP = false;
}

bool rtspHandler::setHandler(string URI, string rtspVer, int port, bool enableUDP)
{
	this->URI = URI;
	this->rtspVersion = rtspVersion;

	//ע�⣺ֻ����ż���˿ںš���������������˿ںţ�������Զ������1������65535�������������
	if (port % 2 != 0)
	{
		streamingPort = (port + 1) % 65535;
	}
	else
	{
		streamingPort = port;
	}

	this->enableUDP = enableUDP;

	return true;
}

string rtspHandler::getHandlerInfo()
{
	string msg;

	/*
		Tips. ������Ŀǰ����Ψһ�ʺ�ʹ����Ŀ��������������
		��Ȼ��ֹһ�����ã���ֻ��������Щ������Ŀ���ˣ��ɶ������Ϻ��𣡣������ǳ���
	*/

	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP�汾��%s\r\n�Ự�ţ�session����%s\r\n�˿ںţ�%d\r\n���䷽ʽ��%s\r\n",
		rtspVersion,
		session,
		streamingPort,
		enableUDP ? "UDP" : "TCP");

	return msg;
}

/*
	�ͻ���RTSP����������

	TODO��
	��Ҫ�����·���������switch������ӷ�֧����

	�Ҿ���û��Ҫʹ�ù���ģʽ���ͻ��˱����ǱȽϼ򵥵�
*/
string rtspHandler::encodeMsg(int method)
{
	string msg;
	msg.resize(BUF_SIZE);

	/*
		Ԥƴ��һ������ͷ��
		METHOD RTSP/1.0
		CSeq: X
		����ֻ��Ϊ�����̴�����ѡ�����Ϊ��Ҫ��
	*/

	string reqLine = " " + URI + " RTSP/" + rtspVersion + "\r\nCSeq: " + to_string(seqNum) + "\r\n";

	switch (method)
	{
	case OPTIONS:
	{
		/*Example Message*/

		/*

		OPTIONS rtsp://example.com/test.mp3 RTSP/1.0
		CSeq: 302

		*/

		msg = "OPTIONS" + reqLine;

		break;
	}
	case DESCRIBE:
	{
		/*Example Message*/

		/*

		DESCRIBE rtsp://example.com/test.mp3 RTSP/1.0
		CSeq: 302
		Accept: application/sdp

		*/
		msg = "DESCRIBE" + reqLine + "Accept: application/sdp\r\n";

		break;
	}
	case SETUP:
	{
		/*Example Message*/

		/*

		SETUP rtsp://example.com/test.mp3 RTSP/1.0
		CSeq: 302
		Transport: RTP/AVP;unicast;client_port=8554-8555

		*/

		sprintf_s((char *)msg.data(), BUF_SIZE, "Transport: RTP/AVP%s;unicast;client_port=%d-%d\r\n",
			enableUDP ? "" : "/TCP",
			streamingPort,
			streamingPort + 1);

		msg = "SETUP" + reqLine + msg;

		break;
	}
	case PLAY:
	{
		/*Example Message*/

		/*

		PLAY rtsp://example.com/test.mp3 RTSP/1.0
		CSeq: 302
		Session: 12345678
		Range: npt=0.000-

		*/

		sprintf_s((char *)msg.data(), BUF_SIZE, "Session: %s\r\nRange: npt=0.000-\r\n", session.c_str());

		msg = "PLAY" + reqLine + msg;

		break;
	}
	case TEARDOWN:
	{
		/*Example Message*/

		/*

		TEARDOWN rtsp://example.com/test.mp3 RTSP/1.0
		CSeq: 302
		Session: 12345678

		*/

		sprintf_s((char *)msg.data(), BUF_SIZE, "Session: %s\r\n", session);

		msg = "TEARDOWN" + reqLine + msg;

		break;
	}
	case GET_PARAMETER:
	{
		/*Example Message*/

		/*

		GET_PARAMETER rtsp://example.com/test.mp3 RTSP/1.0
		CSeq: 302
		Session: 12345678

		*/

		sprintf_s((char *)msg.data(), BUF_SIZE, "Session: %s\r\n", session.c_str());

		msg = "GET_PARAMETER" + reqLine + msg;

		break;
	}
	default:
		break;
	}

	//��ƴ�Ӻõ�����ĩβ����һ���س����з�������ȥ������ĳ���
	msg = msg.substr(0, msg.rfind("\n") + 1) + "\r\n";

	//��Ҫ�����к�������
	++seqNum;          

	return msg;
}

/*
	rtsp���������

	�ص㣺��ȡ��Ӧ����Ϣ��������룬�Ự�ţ�������У���������

	�ͻ���������Ҳ�Ǽ򵥵ġ���
*/
int rtspHandler::decodeMsg(string msg)
{
	rtspErrHandler *errHandler = rtspErrHandler::getInstance();

	//�������
	int errCode; 

	//���кţ����˶ԣ������Ҫ
	int sequence;          

	string buf;

	/*
		Ϊ�˽�����ϣ��ǲ��ǲ�Ҫȥ���ǵ����Ƿ���ʲô��Ϣ�Ļظ�
		�о����ܻ���Ҫ���ǰɣ�����������ʱ�Ͳ�������
		�������ﲢ���ܷ��ͣ������֮��һ���ͷ��˰���˭֪���ǲ��Ǳ�������
		��Ŀǰ��Ӧ�ö��ԣ���ʵֻ��Ҫ����������һ���Ǵ�����루��ʵ������Ϣ����Ҳ���˸������ò����һ���ǻỰ��
	*/

	//��ȡ������
	buf = "RTSP/";
	buf += rtspVersion;
	buf = msg.substr(msg.find(buf) + buf.length() + 1, msg.find('\r') - buf.length());
	//buf = buf.substr(0, buf.find(' '));
	//errCode = atoi(buf.c_str());
	errCode = stoi(buf, nullptr, 10);

	if (errCode != 200)
	{
		cout << "RTSP Error: " << errCode << " " << errHandler->getErrMsg(errCode) << endl;
	}


	/*
		��ȡ���к�
		TODO��������ȡ�˵�û��˵����ô�á��÷������ǣ��˶����к�ȷ�����ҷ���ȥ�Ļظ������к�һ����
	*/
	buf = msg.substr(msg.find("CSeq: "));
	buf = buf.substr(6, buf.find("\r") - 6);
	//sequence = atoi(buf.c_str());
	sequence = stoi(buf, nullptr, 10);

	//��ȡ�Ự�ţ�����������ĻỰ��Ϊ����д�룬����У�飩
	if (session.empty())
	{
		if (msg.find("Session: ") != string::npos)session = msg.substr(msg.find("Session: ") + 9, 9);
	}
	else
	{
		//Todo���Ự��У��
		//string ssnTmp = msg.substr(msg.find("Session: "), 8);
		//if...
	}

	return errCode;
}

string rtspHandler::getSession()
{
	return session;
}
