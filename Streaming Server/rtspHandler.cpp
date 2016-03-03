/*--Author：李宏杰--*/

#include "rtspHandler.h"

/*----------------------RTSP错误信息处理器：RTSP 错误码信息查询---------------------*/
/*
	构造函数：写入文件名，建立错误列表
*/
rtspErrHandler::rtspErrHandler()
{
	settingFile = "config/static/rtspErrCodeList.csv";

	buildErrList();
}

rtspErrHandler::rtspErrHandler(string filePath)
{
	settingFile = filePath;

	buildErrList();
}

/*
	函数：建立RTSP错误信息表
	
	错误映射表用CSV格式存储
	CSV格式数据逗号分隔，非常方便，极其好扩展，直接EXCEL编辑即可
*/
void rtspErrHandler::buildErrList()
{
	//文件流对象
	fstream fileStream;

	//以只读方式打开文件
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
		/*
			成功打开文件，则一直读取到文件结束
			按行读取并解析
		*/
		while (!fileStream.eof())
		{
			getline(fileStream, buf);

			//已修复：如果表尾有空行，后面会报错
			if (buf.empty() || buf.find_first_not_of(' ') == string::npos)
			{
				break;
			}

			/*
				有内容，则解析之，先找逗号位置，前后分别处理
			*/

			commaPos = buf.find(',');

			errCode = stoi(buf.substr(0, commaPos), nullptr, 10);

			msg = buf.substr(commaPos + 1, buf.length() - commaPos);

			//插入Key-Value对
			errCodeList.insert(make_pair(errCode, msg));
		}
	}
}

/*
	输入错误码，通过查表(std::map)的方式返回错误信息字符串
*/
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
/*
	函数：获取NTP时间（距1900.1.1 00:00:00）
	返回unsigned long格式的时间（单位：秒）
*/
unsigned long NTPTimeGenerator::getNTPTime()
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

/*
	获取NTP时间（Unix版本）（距1970.1.1 00:00:00）
	返回unsigned long格式的时间（单位：秒）
*/
unsigned long NTPTimeGenerator::getNTPTimeUnix()
{
	unsigned long time = 0;

	const unsigned long seventyYear = 2208988800;

	time = getNTPTime() - seventyYear;

	return time;
}

/*----------------------SDP编码器---------------------*/
/*
	构造函数
	这里就写一个文件头
*/
sdpEncoder::sdpEncoder()
{
	proVer = "v=0\r\n";
}

/*
	SDP编码函数
	SDP协议细节，请参阅RFC2327文档：http://tools.ietf.org/html/rfc2327
*/
string sdpEncoder::encodeMsg()
{
	/*
		Session description
		v = (protocol version)
		o = (owner / creator and session identifier).
		s = (session name)
		i = *(session information)
		u = *(URI of description)
		e = *(email address)
		p = *(phone number)
		c = *(connection information - not required if included in all media)
		b = *(bandwidth information)
		One or more time descriptions(see below)
		z = *(time zone adjustments)
		k = *(encryption key)
		a = *(zero or more session attribute lines)
		Zero or more media descriptions(see below)

		Time description
		t = (time the session is active)
		r = *(zero or more repeat times)

		Media description
		m = (media name and transport address)
		i = *(media title)
		c = *(connection information - optional if included at session - level)
		b = *(bandwidth information)
		k = *(encryption key)
		a = *(zero or more media attribute lines)

		*****An example SDP description is :*****

		v = 0
		o = mhandley 2890844526 2890842807 IN IP4 126.16.64.4
		s = SDP Seminar
		i = A Seminar on the session description protocol
		u = http://www.cs.ucl.ac.uk/staff/M.Handley/sdp.03.ps
		e = mjh@isi.edu(Mark Handley)
		c = IN IP4 224.2.17.12 / 127
		t = 2873397496 2873404696
		a = recvonly
		m = audio 49170 RTP / AVP 0
		m = video 51372 RTP / AVP 31
		m = application 32416 udp wb
		a = orient:portrait
	*/

	owner = "o=guest " + to_string(ntpTime.getNTPTimeUnix()) + " " + to_string(ntpTime.getNTPTimeUnix()) + " " + "IN IP4 www.rayion.com\r\n";

	sessionName = "s=streaming Test\r\n";

	timeActive = "t=" + to_string(ntpTime.getNTPTimeUnix()) + " \r\n";

	sdpMsg = proVer + owner + sessionName + timeActive;

	//去掉末尾多余的空格
	sdpMsg = sdpMsg.substr(0, sdpMsg.rfind('\n'));

	return sdpMsg;
}

/*----------------------随机会话号生成器---------------------*/
sessionGenerator::sessionGenerator()
{
	session.resize(9);
}

/*
	会话号生成函数
	这里直接使用NTP时间戳来作为会话号

	TODO：
	使用时间戳是否合理？时间戳能保证唯一性么？是否有必要写个真正的随机？
	临时的措施是：使用Sleep(X)来保证唯一，但是这样每个X秒才能分配一个会话号了
*/
string sessionGenerator::getSessionID()
{
	unsigned long time = ntpTime.getNTPTimeUnix();

	//要进行进制转换，必然是得sprintf_s的节奏？
	sprintf_s((char *)session.data(), 9, "%X", time);

	//临时措施，保证会话号唯一
	Sleep(50);

	return session;
}

/*----------------------客户端信息维护---------------------*/
/*
	构造函数：给一个默认设置

	默认端口号：80（信令），8554（RTSP）
	默认不开启UDP（即使用TCP）
	客户端列表首值定为（0x00000000， NULL）
*/
clientList::clientList()
{
	PerClientData defaultData;
	defaultData.streamingPort = 80;
	defaultData.srvPort = 8554;
	defaultData.enableUDP = false;

	unsigned long session = 0x00000000;

	client.insert(make_pair(session, defaultData));
}

/*
	函数：添加客户端信息，返回值为0
	首先查询是否已存在，若不存在则插入

	TODO：返回值设为错误码
*/
int clientList::addClient(unsigned long session, PerClientData clientData)
{
	if (client.find(session) != client.end())
	{
		client.insert(make_pair(session, clientData));
	}

	return 0;
}

/*
	函数：查询客户端是否存在，若不存在则返回值为0，否则返回会话号

	TODO：返回值加入错误码
*/
unsigned long clientList::searchClient(unsigned long session)
{
	map<unsigned long, PerClientData>::iterator iter;

	iter = client.find(session);

	if (iter != client.end())
	{
		return session;
	}
	else
	{
		return 0;
	}
}

/*
	函数：获取客户端信息
	首先查找，若存在则返回客户端信息结构体，否则返回空（即列表的第一个值）
*/
PerClientData clientList::getClientInfo(unsigned long session)
{
	map<unsigned long, PerClientData>::iterator iter;

	iter = client.find(session);

	if (iter != client.end())
	{
		return iter->second;
	}
	else
	{
		return client.begin()->second;
	}
}

/*
	函数：删除客户端信息，返回值为0

	TODO：返回值设为错误码
*/
int clientList::removeClient(unsigned long session)
{
	if (client.find(session) != client.end())
	{
		client.erase(session);
	}

	return 0;
}

/*----------------------RTSP连接处理器---------------------*/

rtspHandler *rtspHandler::instance = new rtspHandler();

/*
	构造函数

	默认值：
	RTSP版本号为1.0，端口号为8554
	可用RTSP方法，直接写死了
	目前可用的方法是，OPTIONS，DESCRIBE，SETUP，PLAY，GET_PARAMETER，TEARDOWN
*/
rtspHandler::rtspHandler()
{
	//版本号固定好
	rtspVersion = "1.0";

	//URI要固定下来，直接获取服务器地址信息并设定好，用于解码校验
	//URI = "";

	srvPort = 8554;

	/*
		可用的RTSP方法，不人工设置了，直接写死可好？
		先全部设置为假，后面把可用的调为真
		TODO：增删这里的条目，已设置可用方法，或是写一个函数设置
	*/

	rtspMethod = { "", "OPTIONS","DESCRIBE","SETUP","PLAY","GET_PARAMETER","PAUSE","TEARDOWN","ANNOUNCE","RECORD","REDIRECT","SET_PARAMETER" };
	availableMethod.resize(rtspMethod.size());

	for (int i = 0; i < availableMethod.size(); ++i)availableMethod[i] = false;

	availableMethod[OPTIONS] = true;
	availableMethod[DESCRIBE] = true;
	availableMethod[SETUP] = true;
	availableMethod[PLAY] = true;
	availableMethod[GET_PARAMETER] = true;
	availableMethod[TEARDOWN] = true;
}

/*
	函数：设置服务器属性，返回值为0
	目前只需要设置端口号
*/
int rtspHandler::srvConfig(string URI, unsigned int srvPort)
{
	this->srvPort = srvPort;

	return 0;
}

rtspHandler* rtspHandler::getInstance()
{
	return instance;
}

/*
	函数：返回错误信息，返回值为错误信息字符串
*/
string rtspHandler::getErrMsg(int code)
{
	return errHandler.getErrMsg(code);
}

/*
	函数：返回rtsp处理器的当前信息，即“RTSP版本：[版本号]”
	如：RTSP版本：1.0
*/
string rtspHandler::getHandlerInfo()
{
	string msg;

	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP版本：%s\r\n", rtspVersion);

	return msg;
}

/*
	编解码方法

	160303，核验确认逻辑上没有任何问题
*/
string rtspHandler::msgCodec(string msg)
{
	string response;

	string request, URI, rtspVersion, seqNum;

	string paddingMsg, tmp;

	/*
		样例信息：

		PLAY rtsp://example.com/test.mp3 RTSP/1.0
		CSeq: 302
		Session: 12345678
		Range: npt=0.000-

		结构可归纳为：

		CMD URI Version
		CSeq: int
		MSG PADDING
	*/

	/*
		循环解析大法：截取信令第一个字段->去掉第一个字段

		info = msg.substr(0,msg.find(空格或换行);
		msg=msg.substr(info.length()+1或者2);
	*/

	/*
		1.获得指令，并且转为数字编号
	*/

	request = msg.substr(0, msg.find(' '));
	
	msg = msg.substr(request.length() + 1);

	int req;

	for (int i = 0; i < rtspMethod.size(); ++i)
	{
		if (request == rtspMethod[i])
		{
			req = i;
			break;
		}
	}

	/*
		2.获得URI地址和RTSP版本号
		残余部分到第一个空格之间为URI
		裁掉URI，残余部分到第一个换行之间为版本号

		Todo:
		检查URI是否合法，不合法则返回相应错误信息（404,Not Found）
	*/

	URI = msg.substr(0, msg.find(' '));

	msg = msg.substr(URI.length() + 1);

	rtspVersion = msg.substr(0, msg.find('\r'));

	msg = msg.substr(rtspVersion.length() + 2);

	/*
		3. 获得序列号
		注意：服务器其实根本不需要具体编号为多少，只需要把这一行原样返回即可
		所以整行复制
	*/

	seqNum = msg.substr(0, msg.find('\n') + 1);

	msg = msg.substr(seqNum.length());

	/*
		4. 获得PADDING信息，根据指令类型做出针对性解析与答复
		!!也就是说，下面编解码一起了
	*/

	//回令的错误码，初始化为200：OK
	int errCode = 200; 

	paddingMsg = msg;

	//根据指令分类处理
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

		//TODO：其它错误情况？

		response = generateCMDLine(errCode) + seqNum;

		if (errCode != 200)
		{
			break;
		}
		
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

		/*
			TODO：首先检查资源是否存在，这块就在这里一个函数
		*/

		/*
			然后解析客户端是否接受SDP
			不接受就直接返回错误，接受就编码回发SDP信息
		*/

		if (paddingMsg.find("sdp") == string::npos)
		{
			//406: Not Acceptable
			errCode = 406;  
		}

		response = generateCMDLine(errCode) + seqNum;

		if (errCode != 200)
		{
			break;
		}

		//下面是正常的信息内容

		//填写日期
		response += generateTimeLine();

		/*
			下面是SDP信息
		*/

		tmp = sdpHandler.encodeMsg();

		response = response + "Content-Type: application/sdp\r\nContent-Length: " + to_string(tmp.length()) + "\r\n\r\n" + tmp;

		break;
	}
	case SETUP:
	{
		/*Example Message*/
		/*
			C->S: 
			SETUP rtsp ://example.com/foo/bar/baz.rm RTSP/1.0
			CSeq : 302
			Transport : RTP / AVP; unicast; client_port = 4588 - 4589

			S->C: 
			RTSP / 1.0 200 OK
			CSeq : 302
			Date : 23 Jan 1997 15 : 35 : 06 GMT
			Session : 47112344
			Transport : RTP / AVP; unicast;
			client_port = 4588 - 4589; server_port = 6256 - 6257
		*/

		//TODO：其它错误情况？

		response = generateCMDLine(errCode) + seqNum;

		if (errCode != 200)
		{
			break;
		}

		//填写日期
		response += generateTimeLine();

		/*
			生成会话号Session
		*/

		tmp = sessGenerator.getSessionID();

		unsigned long session = stol(tmp);

		response = response + "Session: " + tmp + "\r\n";

		/*
			首先解析客户端请求的传送方式和端口号，保存客户端连接信息，然后回发会话号
			还有个关键是……
			解析的信息是RTP包才用的，不过现在不用了，先存着吧
			然后，这里作为信令部分，整行复制返回即可
		*/

		PerClientData clientData;

		//解析出客户端端口
		clientData.streamingPort = stoi(msg.substr(msg.rfind("="), msg.rfind("-")));

		//确定是否用UDP
		if (msg.find("UDP") != string::npos)
		{
			clientData.enableUDP = true;
		}
		else
		{
			clientData.enableUDP = false;
		}

		//把客户端加入列表中
		clientManager.addClient(session, clientData);

		response = response + msg + "; server_port: " + to_string(srvPort) + "-" + to_string(srvPort + 1) + "\r\n";		
		
		break;
	}
	case PLAY:
	{
		/*Example Message*/
		/*
			C->S: 
			PLAY rtsp://audio.example.com/meeting.en RTSP/1.0
			CSeq: 835
			Session: 12345678
			Range: clock=19961108T142300Z-19961108T143520Z

			S->C: 
			RTSP/1.0 200 OK
			CSeq: 835
			Date: 23 Jan 1997 15:35:06 GMT
		*/

		/*
			首先解析Session，判断是否合法
			不合法返回相应错误：454,Session Not Found
		*/

		unsigned long session = stol(msg.substr(msg.find("Session:") + 8));

		if (clientManager.searchClient(session) != 0)
		{
			/*
				TODO：会话号正确，这里就应该调用播放器播放了

				在现在的设计下，PLAY信令的含义为，使得服务端能够与该客户端进行播放行为，响应按键放图
				所以这里TMD啥都不用写……事实上SETUP之后就可以交互了……
			*/

		}
		else
		{
			errCode = 454;
		}

		response = generateCMDLine(errCode) + seqNum;

		if (errCode != 200)
		{
			break;
		}

		//填写日期
		response += generateTimeLine();

		break;
	}
	case TEARDOWN:
	{
		/*Example Message*/
		/*
			C->S: 
			TEARDOWN rtsp://example.com/fizzle/foo RTSP/1.0
			CSeq: 892
			Session: 12345678

			S->C: 
			RTSP/1.0 200 OK
			CSeq: 892
		*/

		/*
			终于想到一种错误情况……你不能TEARDOWN一个不存在的会话啊
			454,Session Not Found
		*/

		unsigned long session = stol(msg.substr(paddingMsg.find("session") + 8));

		if (clientManager.searchClient(session) == 0)
		{
			errCode = 454;
		}

		response = generateCMDLine(errCode) + seqNum;

		if (errCode != 200)
		{
			break;
		}

		//移除客户端信息
		clientManager.removeClient(session);

		break;
	}
	default:
	{
		//能进到这里说明方法不可用嘛，所以呢……405,Method Not Allowed

		errCode = 405;

		response = generateCMDLine(errCode) + seqNum;

		break;
	}
	}

	/*
		由于之前有些地方直接开大缓存存信令
		所以最后要加上一步，即去掉多余的后缀空格
	*/
	response = response.substr(0, response.rfind('\n'));

	return response;
}

//生成时间："Date: 23 Jan 1997 15:35:06 GMT"
string rtspHandler::generateTimeLine()
{
	string msg;

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);

	msg.resize(BUF_SIZE);

	sprintf_s((char *)msg.data(), BUF_SIZE, "Date: %02d %s %s %02d:%02d:%02d GMT\r\n",
		sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	msg = msg.substr(0, msg.rfind('\n'));

	return msg;
}

//生成命令行（第一行）：版本号+错误码+错误信息
string rtspHandler::generateCMDLine(int errCode)
{
	string msg;

	msg = "RTSP/" + rtspVersion + " " + to_string(errCode) + " " + getErrMsg(errCode) + "\r\n";

	return msg;
}