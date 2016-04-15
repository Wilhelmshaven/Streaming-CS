/*--Author：李宏杰--*/

#include "rtspHandler.h"

#include "errHandler.h"

/*----------------------RTSP连接处理器---------------------*/
rtspHandler *rtspHandler::instance = new rtspHandler();

rtspHandler* rtspHandler::getInstance()
{
	return instance;
}

/*
	RTSP处理器构造函数

	默认值：
	序列号（Sequence number）：1
*/
rtspHandler::rtspHandler()
{
	//初始化序列号为1
	seqNum = 1;          
	
	rtspVersion = "1.0";

	streamingPort = 8554;

	enableUDP = false;
}

bool rtspHandler::setHandler(string URI, string rtspVer, int port, bool enableUDP)
{
	this->URI = URI;
	this->rtspVersion = rtspVersion;

	//注意：只接受偶数端口号。因此若输入奇数端口号，这里会自动帮你加1（并对65535求余以免溢出）
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
		Tips. 这是我目前发现唯一适合使用三目运算符的情况……
		当然不止一处适用，我只是讨厌那些滥用三目的人，可读性至上好吗！（而不是长度
	*/

	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP版本：%s\r\n会话号（session）：%s\r\n端口号：%d\r\n传输方式：%s\r\n",
		rtspVersion,
		session,
		streamingPort,
		enableUDP ? "UDP" : "TCP");

	return msg;
}

/*
	客户端RTSP编码器方法

	TODO：
	需要增加新方法，就在switch里面添加分支好了

	我觉得没必要使用工厂模式，客户端编码是比较简单的
*/
string rtspHandler::encodeMsg(int method)
{
	string msg;
	msg.resize(BUF_SIZE);

	/*
		预拼接一个公共头：
		METHOD RTSP/1.0
		CSeq: X
		仅仅只是为了缩短代码而已……因为都要的
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

	//在拼接好的信令末尾补上一个回车换行符，并且去掉多余的长度
	msg = msg.substr(0, msg.rfind("\n") + 1) + "\r\n";

	//重要！序列号自增！
	++seqNum;          

	return msg;
}

/*
	rtsp信令解码器

	重点：提取对应的信息，如错误码，会话号，并用来校验或者其它

	客户端这块相对也是简单的……
*/
int rtspHandler::decodeMsg(string msg)
{
	rtspErrHandler *errHandler = rtspErrHandler::getInstance();

	//错误代码
	int errCode; 

	//序列号，供核对，如果需要
	int sequence;          

	string buf;

	/*
		为了降低耦合，是不是不要去考虑到底是发了什么消息的回复
		感觉可能还是要考虑吧，不过现在暂时就不考虑了
		而且这里并不管发送，你编码之后不一定就发了啊，谁知道是不是编码来玩
		就目前的应用而言，其实只需要解析两个，一个是错误代码（其实错误信息这里也有了根本不用查表），一个是会话号
	*/

	//提取错误码
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
		提取序列号
		TODO：这里提取了但没有说明怎么用。用法可以是，核对序列号确认是我发出去的回复（序列号一样）
	*/
	buf = msg.substr(msg.find("CSeq: "));
	buf = buf.substr(6, buf.find("\r") - 6);
	//sequence = atoi(buf.c_str());
	sequence = stoi(buf, nullptr, 10);

	//提取会话号（如果处理器的会话号为空则写入，否则校验）
	if (session.empty())
	{
		if (msg.find("Session: ") != string::npos)session = msg.substr(msg.find("Session: ") + 9, 9);
	}
	else
	{
		//Todo：会话号校验
		//string ssnTmp = msg.substr(msg.find("Session: "), 8);
		//if...
	}

	return errCode;
}

string rtspHandler::getSession()
{
	return session;
}
