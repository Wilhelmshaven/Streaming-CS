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

	//---------------根据指令做出个性解析与答复----------------//
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

		//日期，类型，长度

		//下面是SDP信息

		break;
	}
	default:
		break;
	}

	return response;
}

