#include "rtspHandler.h"

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

/*----------------------NTP时间戳获取类---------------------*/
//获取NTP时间（距1900.1.1 00:00:00）
unsigned long NTPTime::getNTPTime()
{
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);

	unsigned long time = 0;

	long tmp[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

	time += (sysTime.wYear - 1900) * 365;
	for (int i = 1900; i < sysTime.wYear; ++i)
	{
		if (i % 4 == 0 && i != 1900)++time;
	}

	for (int i = 1; i < sysTime.wMonth; ++i)time += tmp[i];

	time += sysTime.wDay - 1;

	time *= 86400;
	time += (sysTime.wHour - 1) * 3600;
	time += (sysTime.wMinute - 1) * 60;
	time += sysTime.wSecond;

	return time;
}

//获取NTP时间（Unix版本）（距1970.1.1 00:00:00）
unsigned long NTPTime::getNTPTimeUnix()
{
	unsigned long time = 0;

	const unsigned long seventyYear = 2208988800;

	time = getNTPTime() - seventyYear;

	return time;
}

/*----------------------SDP编码器---------------------*/
sdpEncoder::sdpEncoder()
{
	proVer = "v=0\r\n";
}

string sdpEncoder::encodeMsg()
{
	//  Session description
	//	v = (protocol version)
	//	o = (owner / creator and session identifier).
	//	s = (session name)
	//	i = *(session information)
	//	u = *(URI of description)
	//	e = *(email address)
	//	p = *(phone number)
	//	c = *(connection information - not required if included in all media)
	//	b = *(bandwidth information)
	//	One or more time descriptions(see below)
	//	z = *(time zone adjustments)
	//	k = *(encryption key)
	//	a = *(zero or more session attribute lines)
	//	Zero or more media descriptions(see below)

	//	Time description
	//	t = (time the session is active)
	//	r = *(zero or more repeat times)

	//	Media description
	//	m = (media name and transport address)
	//	i = *(media title)
	//	c = *(connection information - optional if included at session - level)
	//	b = *(bandwidth information)
	//	k = *(encryption key)
	//	a = *(zero or more media attribute lines)

	//An example SDP description is :

	//v = 0
	//o = mhandley 2890844526 2890842807 IN IP4 126.16.64.4
	//s = SDP Seminar
	//i = A Seminar on the session description protocol
	//u = http://www.cs.ucl.ac.uk/staff/M.Handley/sdp.03.ps
	//e = mjh@isi.edu(Mark Handley)
	//c = IN IP4 224.2.17.12 / 127
	//t = 2873397496 2873404696
	//a = recvonly
	//m = audio 49170 RTP / AVP 0
	//m = video 51372 RTP / AVP 31
	//m = application 32416 udp wb
	//a = orient:portrait
	string tmp;

	owner = "o=guest " + to_string(ntpTime.getNTPTimeUnix()) + " " + to_string(ntpTime.getNTPTimeUnix()) + " " + "IN IP4 www.rayion.com\r\n";

	sessionName = "s=streaming Test\r\n";

	timeActive = "t=" + to_string(ntpTime.getNTPTimeUnix()) + " 0\r\n";

	sdpMsg = proVer + owner + sessionName + timeActive;

	return sdpMsg;
}

/*----------------------随机会话号生成器---------------------*/
sessionGenerator::sessionGenerator()
{
	session.resize(9);
}
//这里直接使用时间戳来作为会话号
string sessionGenerator::getSessionID()
{
	unsigned long time = ntpTime.getNTPTimeUnix();

	sprintf_s((char *)session.data(), 9, "%X", time);

	return session;
}

/*----------------------RTSP连接处理器---------------------*/
//直接初始化
rtspHandler *rtspHandler::instance = new rtspHandler();

//RTSP处理器构造函数
rtspHandler::rtspHandler()
{
	//版本号固定好
	rtspVersion = "1.0";

	//URI要固定下来，直接获取服务器地址信息并设定好，用于解码校验
	//URI = "";

	//可用的RTSP方法，不人工设置了，直接写死可好？
	rtspMethod = { "", "OPTIONS","DESCRIBE","SETUP","PLAY","GET_PARAMETER","PAUSE","TEARDOWN","ANNOUNCE","RECORD","REDIRECT","SET_PARAMETER" };
	availableMethod.resize(rtspMethod.size());
	for (int i = 0; i < availableMethod.size(); ++i)availableMethod[i] = false;

	//开始设置可用的方法
	availableMethod[OPTIONS] = true;
	availableMethod[DESCRIBE] = true;
	availableMethod[SETUP] = true;
	availableMethod[PLAY] = true;
	availableMethod[GET_PARAMETER] = true;
	availableMethod[TEARDOWN] = true;
}

//一系列返回相关信息的函数
rtspHandler* rtspHandler::getInstance()
{
	return instance;
}

//返回错误信息
string rtspHandler::getErrMsg(int code)
{
	return errHandler.getErrMsg(code);
}

//返回rtsp处理器的当前信息
string rtspHandler::getHandlerInfo()
{
	string msg;
	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP版本：%s\r\n", rtspVersion);
	return msg;
}

//编解码器
string rtspHandler::msgCodec(string msg)
{
	string response;

	string request, URI, rtspVersion, seqNum;
	string paddingMsg;
	string tmp;

	/*Example Input Message*/
	/*
	PLAY rtsp://example.com/test.mp3 RTSP/1.0
	CSeq: 302
	Session: 12345678
	Range: npt=0.000-

	that is->

	CMD URI Version
	CSeq: int
	MSG PADDING
	*/

	//1. Get CMD
	request = msg.substr(0, msg.find(' '));
	
	//2. Get URI and rtsp version
	tmp = msg.substr(request.length());
	rtspVersion = tmp.substr(tmp.find(' '));
	URI = tmp.substr(0, tmp.find(' '));

	//Todo:检查URI是否合法，不合法则返回相应错误信息

	//3. Get sequence number
	//直接复制整行就行，并不关心具体是多少，原样转回即可
	//注意要留下\r\n（当然也可以要么就都不留，改成find \r，而不是find \n + 1)\）
	tmp = msg.find("CSeq");
	seqNum = tmp.substr(0, tmp.find('\n') + 1);

	//4. Get PADDING MESSAGE
	//首先把指令转成数字
	int req;
	for (int i = 0; i < rtspMethod.size(); ++i)
	{
		if (request == rtspMethod[i])
		{
			req = i;
			break;
		}
	}

	//---------------根据指令做出针对性解析与答复----------------//

	paddingMsg = msg.find(msg.find(seqNum) + seqNum.length());
	int errCode;   //回令的错误码
	errCode = 200; //初始化为200：OK

	//先写命令行，版本号加错误代码和错误消息
	string cmdLine;
	cmdLine = "RTSP/" + rtspVersion + " ";

	switch (req)
	{
	case OPTIONS:
	{
		//OPTION：直接回复可用方法即可

		/*Example Message*/
		/* 
		C->S
		OPTIONS * RTSP/1.0
		CSeq: 1
		Require: implicit-play
		Proxy-Require: gzipped-messages

		S->C
		RTSP/1.0 200 OK
		CSeq: 1
		Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE
		*/
		cmdLine += to_string(errCode) + " " + errHandler.getErrMsg(errCode) + "\r\n";
		response = cmdLine + seqNum;
		
		//返回可用方法
		tmp = "Public: ";
		for (int i = 0; i < availableMethod.size(); ++i)
		{
			if (availableMethod[i])
			{
				tmp = tmp + rtspMethod[i] + ", ";
			}
		}
		//去掉末尾多余的逗号和空格
		tmp.pop_back();
		tmp.pop_back(); 
		tmp += "\r\n";

		response += tmp;

		break;
	}
	case DESCRIBE:
	{
		//返回媒体描述信息（SDP）
		/*Example Message*/
		/*
		C->S
		DESCRIBE rtsp://server.example.com/fizzle/foo RTSP/1.0
		CSeq: 312
		Accept: application/sdp, application/rtsl, application/mheg

		S->C
		RTSP/1.0 200 OK
		CSeq: 312
		Date: 23 Jan 1997 15:35:06 GMT
		Content-Type: application/sdp
		Content-Length: 376

		v=0
		o=mhandley 2890844526 2890842807 IN IP4 126.16.64.4
		s=SDP Seminar
		i=A Seminar on the session description protocol
		u=http://www.cs.ucl.ac.uk/staff/M.Handley/sdp.03.ps
		e=mjh@isi.edu (Mark Handley)
		c=IN IP4 224.2.17.12/127
		t=2873397496 2873404696
		a=recvonly
		m=audio 3456 RTP/AVP 0
		m=video 2232 RTP/AVP 31
		m=whiteboard 32416 UDP WB
		a=orient:portrait
		*/

		//首先检查资源是否存在，这块写一个函数

		//然后解析客户端是否接受SDP
		//不接受就直接返回错误，接受就编码回发SDP信息
		if (paddingMsg.find("sdp") == string::npos)errCode = 406;  //406: Not Acceptable

		cmdLine += to_string(errCode) + " " + errHandler.getErrMsg(errCode) + "\r\n";
		response = cmdLine + seqNum;

		if (errCode != 200)break;

		//下面是正常的信息内容

		//日期，类型，长度  Date: 23 Jan 1997 15:35:06 GMT
		SYSTEMTIME sysTime;
		GetSystemTime(&sysTime);

		tmp.resize(BUF_SIZE);
		sprintf_s((char *)tmp.data(), BUF_SIZE, "Date: %02d %s %s %02d:%02d:%02d GMT\r\n",
			sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		tmp = tmp.substr(0, tmp.rfind('\n'));
		tmp += "Content-Type: application/sdp\r\nContent-Length: ";

		response += tmp;

		//下面是SDP信息
		tmp = sdpHandler.encodeMsg();
		response = response + to_string(tmp.length()) + "\r\n\r\n";

		response += tmp;

		break;
	}
	case SETUP:
	{
		/*Example Message*/
		/*
		C->S: SETUP rtsp ://example.com/foo/bar/baz.rm RTSP/1.0
		CSeq : 302
		Transport : RTP / AVP; unicast; client_port = 4588 - 4589

		S->C: RTSP / 1.0 200 OK
		CSeq : 302
		Date : 23 Jan 1997 15 : 35 : 06 GMT
		Session : 47112344
		Transport : RTP / AVP; unicast;
		client_port = 4588 - 4589; server_port = 6256 - 6257
		*/

		//首先解析客户端请求的传送方式和端口号，然后回发会话号

		break;
	}
	default:
		break;
	}

	return response;
}

