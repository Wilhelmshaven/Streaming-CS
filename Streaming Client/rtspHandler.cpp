#include "rtspHandler.h"

/*----------------------RTSP连接处理器：RTSP 错误码信息查询---------------------*/
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
	rtspVersion = MAKEWORD(1, 0);
	streamingPort = 54236;
	enableUDP = false;
}

//返回错误信息
string rtspHandler::getErrMsg(int code)
{
	return errHandler.getErrMsg(code);
}

//改变流媒体参数（就两个就好了，是否UDP以及端口）
bool rtspHandler::setHandler(WORD rtspVer, int port, bool enableUDP)
{
	if(session.empty())rtspVersion = rtspVer;
	streamingPort = port;
	this->enableUDP = enableUDP;

	return true;
}

//返回rtsp处理器的当前信息
string rtspHandler::getHandlerInfo()
{
	string msg;
	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP版本：%d.%d\r\n会话号（session）：%s\r\n端口号：%d\r\n传输方式：%s\r\n",
		rtspVersion >> 8 & 0xff, rtspVersion & 0xff,
		session,
		streamingPort,
		enableUDP ? "UDP" : "TCP");
	return msg;
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
			msg = buf.substr(commaPos + 1, buf.length() - code.length());
			errCode = atoi(code.c_str());

			errCodeList.insert(make_pair(errCode, msg));
		}
	}
}

//查表返回对应信息
string rtspErrHandler::getErrMsg(int code)
{
	string msg = errCodeList.find(code)->second;
	if (msg.empty())msg = "Invalid Error Code";

	return msg;
}