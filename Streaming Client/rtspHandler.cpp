#include "rtspHandler.h"

/*----------------------RTSP连接处理器---------------------*/
//直接初始化
rtspHandler *rtspHandler::instance = new rtspHandler();

//一系列返回相关信息的函数
rtspHandler* rtspHandler::getInstance()
{
	return instance;
}

//RTSP处理器构造函数
rtspHandler::rtspHandler()
{
	seqNum = 1;                   //初始化序列号为1
	//rtspVersion = MAKEWORD(1, 0);
	rtspVersion = "1.0";
	streamingPort = 8554;
	enableUDP = false;
}

//返回错误信息
string rtspHandler::getErrMsg(int code)
{
	return errHandler.getErrMsg(code);
}

//改变流媒体参数（就三个就好了，地址，是否UDP以及端口）
bool rtspHandler::setHandler(string URI, WORD rtspVer, int port, bool enableUDP)
{
	this->URI = URI;

	sprintf_s((char *)rtspVersion.data(), 4, "%d.%d", rtspVer & 0xff, rtspVer >> 8 & 0xff);

	//只接受偶数端口号
	if (port % 2 != 0)streamingPort = (port + 1) % 65535;
	else streamingPort = port;

	this->enableUDP = enableUDP;

	return true;
}

//返回rtsp处理器的当前信息
string rtspHandler::getHandlerInfo()
{
	string msg;
	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP版本：%s\r\n会话号（session）：%s\r\n端口号：%d\r\n传输方式：%s\r\n",
		rtspVersion,
		session,
		streamingPort,
		enableUDP ? "UDP" : "TCP");
	return msg;
}

//rtsp信令编码器（用到的很少就一个函数得了）
string rtspHandler::encodeMsg(int method)
{
	string msg;
	msg.resize(BUF_SIZE);

	//仅仅只是缩短代码而已……
	string reqLine;
	reqLine.resize(BUF_SIZE);
	sprintf_s((char *)reqLine.data(), BUF_SIZE, " %s RTSP/%s\r\nCSeq:%d\r\n", URI.c_str(), rtspVersion.c_str(), seqNum);
	reqLine = reqLine.substr(0, reqLine.rfind('\n'));    //去掉过长的空结尾，否则会影响字符拼接……

	switch (method)
	{
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
		sprintf_s((char *)msg.data(), BUF_SIZE, "Session: %s\r\nRange: npt=0.000-\r\n",
			session.c_str());

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
		sprintf_s((char *)msg.data(), BUF_SIZE, "Session: %s\r\n",
			session);

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
		sprintf_s((char *)msg.data(), BUF_SIZE, "Session: %s\r\n",
			session.c_str());

		msg = "GET_PARAMETER" + reqLine + msg;

		break;
	}
	default:
		break;
	}

	msg = msg.substr(0, msg.find('\0')) + "\r\n";
	++seqNum;          //重要！序列号自增！
	return msg;
}

//rtsp信令解码器
int rtspHandler::decodeMsg(string msg)
{
	int errCode;           //错误代码
	int sequence;          //序列号，供核对，如果需要
	string buf;

	//为了降低耦合，是不是不要去考虑到底是发了什么消息的回复
	//感觉可能还是要考虑吧，不过现在暂时就不考虑了
	//而且这里并不管发送，你编码之后不一定就发了啊，谁知道是不是遍来玩
	//就目前的应用而言，其实只需要解析两个，一个是错误代码（其实错误信息这里也有了根本不用查表），一个是会话号

	//提取错误码
	buf = "RTSP/";
	buf += rtspVersion;
	buf = msg.substr(msg.find(buf) + buf.length() + 1, msg.find('\r') - buf.length());
	//buf = buf.substr(0, buf.find(' '));
	//errCode = atoi(buf.c_str());
	errCode = stoi(buf, nullptr, 10);

	//提取序列号
	buf = msg.substr(msg.find("CSeq: "));
	buf = buf.substr(6, buf.find("\r") - 6);
	//sequence = atoi(buf.c_str());
	sequence = stoi(buf, nullptr, 10);

	//提取会话号（如果处理器的会话号为空则写入，否则校验）
	if (session.empty())
	{
		if (msg.find("Session: ") != string::npos)session = msg.substr(msg.find("Session: ") + 9, 8);
	}
	else
	{
		//string ssnTmp = msg.substr(msg.find("Session: "), 8);
		//if...
	}

	return errCode;
}

/*----------------------RTSP错误信息处理器：RTSP 错误码信息查询---------------------*/
//构造函数：写入文件名，建立错误列表
rtspErrHandler::rtspErrHandler()
{
	settingFile = rtspErrFile;    //To do: change var->rtspErrFile
	buildErrList();
}

//建表
void rtspErrHandler::buildErrList()
{
	//错误映射表用CSV格式存储的
	//逗号分隔，非常方便，极其好扩展，直接EXCEL编辑即可
	fstream fileStream;
	fileStream.open(settingFile.c_str(), ios_base::in);

	string buf, code, msg;
	int errCode;
	int commaPos;

	if (!fileStream.is_open())
	{
		cout << "建立RTSP错误信息表失败：表文件不存在" << endl;
	}
	else
	{
		while (!fileStream.eof())
		{
			getline(fileStream, buf);

			commaPos = buf.find(',');
			code = buf.substr(0, commaPos);
			errCode = stoi(code, nullptr, 10);
			msg = buf.substr(commaPos + 1, buf.length() - code.length());	

			errCodeList.insert(make_pair(errCode, msg));
		}
	}
}

//查表返回对应信息
string rtspErrHandler::getErrMsg(int code)
{
	string msg;

	map<int, string>::iterator iter = errCodeList.find(code);

	if (iter != errCodeList.end())
	{
		msg = iter->second;
	}
	else
	{
		msg = "Invalid Error Code";
	}

	return msg;
}