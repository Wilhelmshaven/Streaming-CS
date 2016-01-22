#include "rtspHandler.h"

/*----------------------RTSP������Ϣ��������RTSP ��������Ϣ��ѯ---------------------*/
//���캯����д���ļ��������������б�
rtspErrHandler::rtspErrHandler()
{
	settingFile = rtspErrFile;    //To do: change var->rtspErrFile
	buildErrList();
}

//����
void rtspErrHandler::buildErrList()
{
	//����ӳ�����CSV��ʽ�洢��
	//���ŷָ����ǳ����㣬�������չ��ֱ��EXCEL�༭����
	fstream fileStream;
	fileStream.open(settingFile.c_str(), ios_base::in);

	string buf, code, msg;
	int errCode;
	int commaPos;

	if (!fileStream.is_open())
	{
		cout << "����RTSP������Ϣ��ʧ�ܣ����ļ�������" << endl;
	}
	else
	{
		while (!fileStream.eof())
		{
			getline(fileStream, buf);

			//Bug fix�������β�п��У�����ᱨ��
			if (buf.empty())break;

			commaPos = buf.find(',');
			errCode = stoi(buf, nullptr, 10);

			msg = buf.substr(commaPos + 1, buf.length() - code.length());

			errCodeList.insert(make_pair(errCode, msg));
		}
	}
}

//����ض�Ӧ��Ϣ
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

/*----------------------NTPʱ�����ȡ��---------------------*/
//��ȡNTPʱ�䣨��1900.1.1 00:00:00��
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

//��ȡNTPʱ�䣨Unix�汾������1970.1.1 00:00:00��
unsigned long NTPTime::getNTPTimeUnix()
{
	unsigned long time = 0;

	const unsigned long seventyYear = 2208988800;

	time = getNTPTime() - seventyYear;

	return time;
}

/*----------------------SDP������---------------------*/
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

/*----------------------����Ự��������---------------------*/
sessionGenerator::sessionGenerator()
{
	session.resize(9);
}

//����ֱ��ʹ��ʱ�������Ϊ�Ự��
string sessionGenerator::getSessionID()
{
	unsigned long time = ntpTime.getNTPTimeUnix();

	sprintf_s((char *)session.data(), 9, "%X", time);

	return session;
}

/*----------------------�ͻ�����Ϣά��---------------------*/
//��һ��Ĭ������
clientList::clientList()
{
	PerClientData defaultData;
	defaultData.streamingPort = 80;
	defaultData.srvPort = 8554;
	defaultData.enableUDP = false;

	unsigned long session = 0x00000000;

	client.insert(make_pair(session, defaultData));
}

//��ӿͻ�����Ϣ
int clientList::addClient(unsigned long session, PerClientData clientData)
{
	if(client.find(session)!=client.end())client.insert(make_pair(session, clientData));

	return 0;
}

//��ѯ�ͻ����Ƿ����
unsigned long clientList::searchClient(unsigned long session)
{
	map<unsigned long, PerClientData>::iterator iter;
	iter = client.find(session);

	if (iter != client.end())
	{
		return session;
	}
	else return 0;
}

//��ȡ�ͻ�����Ϣ
PerClientData clientList::getClientInfo(unsigned long session)
{
	map<unsigned long, PerClientData>::iterator iter;
	iter = client.find(session);

	if (iter != client.end())
	{
		return iter->second;
	}
	else return client.begin()->second;
}

//ɾ���ͻ�����Ϣ
int clientList::removeClient(unsigned long session)
{
	if (client.find(session) != client.end())client.erase(session);

	return 0;
}

/*----------------------RTSP���Ӵ�����---------------------*/
//ֱ�ӳ�ʼ��
rtspHandler *rtspHandler::instance = new rtspHandler();

//RTSP���������캯��
rtspHandler::rtspHandler()
{
	//�汾�Ź̶���
	rtspVersion = "1.0";

	//URIҪ�̶�������ֱ�ӻ�ȡ��������ַ��Ϣ���趨�ã����ڽ���У��
	//URI = "";

	//���õ�RTSP���������˹������ˣ�ֱ��д���ɺã�
	rtspMethod = { "", "OPTIONS","DESCRIBE","SETUP","PLAY","GET_PARAMETER","PAUSE","TEARDOWN","ANNOUNCE","RECORD","REDIRECT","SET_PARAMETER" };
	availableMethod.resize(rtspMethod.size());
	for (int i = 0; i < availableMethod.size(); ++i)availableMethod[i] = false;

	//��ʼ���ÿ��õķ���
	availableMethod[OPTIONS] = true;
	availableMethod[DESCRIBE] = true;
	availableMethod[SETUP] = true;
	availableMethod[PLAY] = true;
	availableMethod[GET_PARAMETER] = true;
	availableMethod[TEARDOWN] = true;
}

//���÷���������
//Ŀǰֻ��Ҫ���ö˿ں�
int rtspHandler::srvConfig(unsigned int srvPort)
{
	this->srvPort = srvPort;

	return 0;
}

//++++++һϵ�з��������Ϣ�ĺ���
//����ʵ��
rtspHandler* rtspHandler::getInstance()
{
	return instance;
}

//���ش�����Ϣ
string rtspHandler::getErrMsg(int code)
{
	return errHandler.getErrMsg(code);
}

//����rtsp�������ĵ�ǰ��Ϣ
string rtspHandler::getHandlerInfo()
{
	string msg;
	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP�汾��%s\r\n", rtspVersion);
	return msg;
}

//�������
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

	//Todo:���URI�Ƿ�Ϸ������Ϸ��򷵻���Ӧ������Ϣ��404,Not Found��

	//3. Get sequence number
	//ֱ�Ӹ������о��У��������ľ����Ƕ��٣�ԭ��ת�ؼ���
	//ע��Ҫ����\r\n����ȻҲ����Ҫô�Ͷ��������ĳ�find \r��������find \n + 1)\��
	tmp = msg.find("CSeq");
	seqNum = tmp.substr(0, tmp.find('\n') + 1);

	//4. Get PADDING MESSAGE
	//���Ȱ�ָ��ת������
	int req;
	for (int i = 0; i < rtspMethod.size(); ++i)
	{
		if (request == rtspMethod[i])
		{
			req = i;
			break;
		}
	}

	//---------------5.����ָ����������Խ������----------------//

	paddingMsg = msg.find(msg.find(seqNum) + seqNum.length());
	int errCode;   //����Ĵ�����
	errCode = 200; //��ʼ��Ϊ200��OK

	//��д�����У��汾�żӴ������ʹ�����Ϣ
	string cmdLine;
	cmdLine = "RTSP/" + rtspVersion + " ";

	switch (req)
	{
	case OPTIONS:
	{
		//OPTION��ֱ�ӻظ����÷�������

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
		
		//���ؿ��÷���
		tmp = "Public: ";
		for (int i = 0; i < availableMethod.size(); ++i)
		{
			if (availableMethod[i])
			{
				tmp = tmp + rtspMethod[i] + ", ";
			}
		}
		//ȥ��ĩβ����Ķ��źͿո�
		tmp.pop_back();
		tmp.pop_back(); 
		tmp += "\r\n";

		response += tmp;

		break;
	}
	case DESCRIBE:
	{
		//����ý��������Ϣ��SDP��
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

		//���ȼ����Դ�Ƿ���ڣ����дһ������

		//Ȼ������ͻ����Ƿ����SDP
		//�����ܾ�ֱ�ӷ��ش��󣬽��ܾͱ���ط�SDP��Ϣ
		if (paddingMsg.find("sdp") == string::npos)errCode = 406;  //406: Not Acceptable

		cmdLine += to_string(errCode) + " " + errHandler.getErrMsg(errCode) + "\r\n";
		response = cmdLine + seqNum;

		if (errCode != 200)break;

		//��������������Ϣ����

		//���ڣ����ͣ�����  Date: 23 Jan 1997 15:35:06 GMT
		SYSTEMTIME sysTime;
		GetSystemTime(&sysTime);

		tmp.resize(BUF_SIZE);
		sprintf_s((char *)tmp.data(), BUF_SIZE, "Date: %02d %s %s %02d:%02d:%02d GMT\r\n",
			sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		tmp = tmp.substr(0, tmp.rfind('\n'));
		tmp += "Content-Type: application/sdp\r\nContent-Length: ";

		response += tmp;

		//������SDP��Ϣ
		tmp = sdpHandler.encodeMsg();
		response = response + to_string(tmp.length()) + "\r\n\r\n";

		response += tmp;

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
		cmdLine += to_string(errCode) + " " + errHandler.getErrMsg(errCode) + "\r\n";
		response = cmdLine + seqNum;

		if (errCode != 200)break;

		//��д����
		SYSTEMTIME sysTime;
		GetSystemTime(&sysTime);

		tmp.resize(BUF_SIZE);
		sprintf_s((char *)tmp.data(), BUF_SIZE, "Date: %02d %s %s %02d:%02d:%02d GMT\r\n",
			sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		tmp = tmp.substr(0, tmp.rfind('\n'));

		response += tmp;

		//���ɻỰ��Session
		unsigned long session;
		tmp = sessGenerator.getSessionID();
		session = stol(tmp);
		tmp = "Session: " + tmp + "\r\n";
		response += tmp;

		//���Ƚ����ͻ�������Ĵ��ͷ�ʽ�Ͷ˿ںţ�Ȼ��ط��Ự��
		string transport = paddingMsg;

		PerClientData clientData;

		tmp = transport.substr(transport.rfind("="), transport.rfind("-"));
		clientData.streamingPort = stoi(tmp);

		if (transport.find("UDP") != string::npos)clientData.enableUDP = true;
		else clientData.enableUDP = false;

		transport = transport + "server_port: " + to_string(srvPort) + "-" + to_string(srvPort + 1) + "\r\n";
		response += transport;

		//�Լ�������ͻ����б�
		clientManager.addClient(session, clientData);
		
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

		//���Ƚ���Session���ж��Ƿ�Ϸ������Ϸ�������Ӧ����454,Session Not Found
		tmp = paddingMsg.substr(paddingMsg.find("Session:") + 8);
		unsigned long session = stol(tmp);
		session = clientManager.searchClient(session);
		if (session != 0)
		{
			tmp = paddingMsg.substr(paddingMsg.find("Session"));
			tmp = tmp.substr(0, tmp.find("\n"));
		}
		else errCode = 454;

		cmdLine += to_string(errCode) + " " + errHandler.getErrMsg(errCode) + "\r\n";
		response = cmdLine + seqNum;

		if (errCode != 200)break;

		//��д����
		SYSTEMTIME sysTime;
		GetSystemTime(&sysTime);

		tmp.resize(BUF_SIZE);
		sprintf_s((char *)tmp.data(), BUF_SIZE, "Date: %02d %s %s %02d:%02d:%02d GMT\r\n",
			sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		tmp = tmp.substr(0, tmp.rfind('\n'));

		response += tmp;

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

		//�Ͽ�����
		cmdLine += to_string(errCode) + " " + errHandler.getErrMsg(errCode) + "\r\n";
		response = cmdLine + seqNum;

		if (errCode != 200)break;

		//�Ƴ��ͻ�����Ϣ
		unsigned long session;
		session = stol(paddingMsg.substr(paddingMsg.find("session") + 8));
		clientManager.removeClient(session);

		break;
	}
	default:
	{
		//�ܽ�������˵������������������ء���405,Method Not Allowed
		errCode = 405;

		cmdLine += to_string(errCode) + " " + errHandler.getErrMsg(errCode) + "\r\n";
		response = cmdLine + seqNum;

		break;
	}
	}

	return response;
}