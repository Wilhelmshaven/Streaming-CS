/*--Author������--*/

#include "rtspHandler.h"

/*----------------------RTSP������Ϣ��������RTSP ��������Ϣ��ѯ---------------------*/
/*
	���캯����д���ļ��������������б�
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
	����������RTSP������Ϣ��
	
	����ӳ�����CSV��ʽ�洢
	CSV��ʽ���ݶ��ŷָ����ǳ����㣬�������չ��ֱ��EXCEL�༭����
*/
void rtspErrHandler::buildErrList()
{
	//�ļ�������
	fstream fileStream;

	//��ֻ����ʽ���ļ�
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
		/*
			�ɹ����ļ�����һֱ��ȡ���ļ�����
			���ж�ȡ������
		*/
		while (!fileStream.eof())
		{
			getline(fileStream, buf);

			//���޸��������β�п��У�����ᱨ��
			if (buf.empty() || buf.find_first_not_of(' ') == string::npos)
			{
				break;
			}

			/*
				�����ݣ������֮�����Ҷ���λ�ã�ǰ��ֱ���
			*/

			commaPos = buf.find(',');

			errCode = stoi(buf.substr(0, commaPos), nullptr, 10);

			msg = buf.substr(commaPos + 1, buf.length() - commaPos);

			//����Key-Value��
			errCodeList.insert(make_pair(errCode, msg));
		}
	}
}

/*
	��������룬ͨ�����(std::map)�ķ�ʽ���ش�����Ϣ�ַ���
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

/*----------------------NTPʱ�����ȡ��---------------------*/
/*
	��������ȡNTPʱ�䣨��1900.1.1 00:00:00��
	����unsigned long��ʽ��ʱ�䣨��λ���룩
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
	��ȡNTPʱ�䣨Unix�汾������1970.1.1 00:00:00��
	����unsigned long��ʽ��ʱ�䣨��λ���룩
*/
unsigned long NTPTimeGenerator::getNTPTimeUnix()
{
	unsigned long time = 0;

	const unsigned long seventyYear = 2208988800;

	time = getNTPTime() - seventyYear;

	return time;
}

/*----------------------SDP������---------------------*/
/*
	���캯��
	�����дһ���ļ�ͷ
*/
sdpEncoder::sdpEncoder()
{
	proVer = "v=0\r\n";
}

/*
	SDP���뺯��
	SDPЭ��ϸ�ڣ������RFC2327�ĵ���http://tools.ietf.org/html/rfc2327
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

	//ȥ��ĩβ����Ŀո�
	sdpMsg = sdpMsg.substr(0, sdpMsg.rfind('\n'));

	return sdpMsg;
}

/*----------------------����Ự��������---------------------*/
sessionGenerator::sessionGenerator()
{
	session.resize(9);
}

/*
	�Ự�����ɺ���
	����ֱ��ʹ��NTPʱ�������Ϊ�Ự��

	TODO��
	ʹ��ʱ����Ƿ����ʱ����ܱ�֤Ψһ��ô���Ƿ��б�Ҫд�������������
	��ʱ�Ĵ�ʩ�ǣ�ʹ��Sleep(X)����֤Ψһ����������ÿ��X����ܷ���һ���Ự����
*/
string sessionGenerator::getSessionID()
{
	unsigned long time = ntpTime.getNTPTimeUnix();

	//Ҫ���н���ת������Ȼ�ǵ�sprintf_s�Ľ��ࣿ
	sprintf_s((char *)session.data(), 9, "%X", time);

	//��ʱ��ʩ����֤�Ự��Ψһ
	Sleep(50);

	return session;
}

/*----------------------�ͻ�����Ϣά��---------------------*/
/*
	���캯������һ��Ĭ������

	Ĭ�϶˿ںţ�80�������8554��RTSP��
	Ĭ�ϲ�����UDP����ʹ��TCP��
	�ͻ����б���ֵ��Ϊ��0x00000000�� NULL��
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
	��������ӿͻ�����Ϣ������ֵΪ0
	���Ȳ�ѯ�Ƿ��Ѵ��ڣ��������������

	TODO������ֵ��Ϊ������
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
	��������ѯ�ͻ����Ƿ���ڣ����������򷵻�ֵΪ0�����򷵻ػỰ��

	TODO������ֵ���������
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
	��������ȡ�ͻ�����Ϣ
	���Ȳ��ң��������򷵻ؿͻ�����Ϣ�ṹ�壬���򷵻ؿգ����б�ĵ�һ��ֵ��
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
	������ɾ���ͻ�����Ϣ������ֵΪ0

	TODO������ֵ��Ϊ������
*/
int clientList::removeClient(unsigned long session)
{
	if (client.find(session) != client.end())
	{
		client.erase(session);
	}

	return 0;
}

/*----------------------RTSP���Ӵ�����---------------------*/

rtspHandler *rtspHandler::instance = new rtspHandler();

/*
	���캯��

	Ĭ��ֵ��
	RTSP�汾��Ϊ1.0���˿ں�Ϊ8554
	����RTSP������ֱ��д����
	Ŀǰ���õķ����ǣ�OPTIONS��DESCRIBE��SETUP��PLAY��GET_PARAMETER��TEARDOWN
*/
rtspHandler::rtspHandler()
{
	//�汾�Ź̶���
	rtspVersion = "1.0";

	//URIҪ�̶�������ֱ�ӻ�ȡ��������ַ��Ϣ���趨�ã����ڽ���У��
	//URI = "";

	srvPort = 8554;

	/*
		���õ�RTSP���������˹������ˣ�ֱ��д���ɺã�
		��ȫ������Ϊ�٣�����ѿ��õĵ�Ϊ��
		TODO����ɾ�������Ŀ�������ÿ��÷���������дһ����������
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
	���������÷��������ԣ�����ֵΪ0
	Ŀǰֻ��Ҫ���ö˿ں�
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
	���������ش�����Ϣ������ֵΪ������Ϣ�ַ���
*/
string rtspHandler::getErrMsg(int code)
{
	return errHandler.getErrMsg(code);
}

/*
	����������rtsp�������ĵ�ǰ��Ϣ������RTSP�汾��[�汾��]��
	�磺RTSP�汾��1.0
*/
string rtspHandler::getHandlerInfo()
{
	string msg;

	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP�汾��%s\r\n", rtspVersion);

	return msg;
}

/*
	����뷽��

	160303������ȷ���߼���û���κ�����
*/
string rtspHandler::msgCodec(string msg)
{
	string response;

	string request, URI, rtspVersion, seqNum;

	string paddingMsg, tmp;

	/*
		������Ϣ��

		PLAY rtsp://example.com/test.mp3 RTSP/1.0
		CSeq: 302
		Session: 12345678
		Range: npt=0.000-

		�ṹ�ɹ���Ϊ��

		CMD URI Version
		CSeq: int
		MSG PADDING
	*/

	/*
		ѭ�������󷨣���ȡ�����һ���ֶ�->ȥ����һ���ֶ�

		info = msg.substr(0,msg.find(�ո����);
		msg=msg.substr(info.length()+1����2);
	*/

	/*
		1.���ָ�����תΪ���ֱ��
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
		2.���URI��ַ��RTSP�汾��
		���ಿ�ֵ���һ���ո�֮��ΪURI
		�õ�URI�����ಿ�ֵ���һ������֮��Ϊ�汾��

		Todo:
		���URI�Ƿ�Ϸ������Ϸ��򷵻���Ӧ������Ϣ��404,Not Found��
	*/

	URI = msg.substr(0, msg.find(' '));

	msg = msg.substr(URI.length() + 1);

	rtspVersion = msg.substr(0, msg.find('\r'));

	msg = msg.substr(rtspVersion.length() + 2);

	/*
		3. ������к�
		ע�⣺��������ʵ��������Ҫ������Ϊ���٣�ֻ��Ҫ����һ��ԭ�����ؼ���
		�������и���
	*/

	seqNum = msg.substr(0, msg.find('\n') + 1);

	msg = msg.substr(seqNum.length());

	/*
		4. ���PADDING��Ϣ������ָ��������������Խ������
		!!Ҳ����˵����������һ����
	*/

	//����Ĵ����룬��ʼ��Ϊ200��OK
	int errCode = 200; 

	paddingMsg = msg;

	//����ָ����ദ��
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

		//TODO���������������

		response = generateCMDLine(errCode) + seqNum;

		if (errCode != 200)
		{
			break;
		}
		
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

		/*
			TODO�����ȼ����Դ�Ƿ���ڣ�����������һ������
		*/

		/*
			Ȼ������ͻ����Ƿ����SDP
			�����ܾ�ֱ�ӷ��ش��󣬽��ܾͱ���ط�SDP��Ϣ
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

		//��������������Ϣ����

		//��д����
		response += generateTimeLine();

		/*
			������SDP��Ϣ
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

		//TODO���������������

		response = generateCMDLine(errCode) + seqNum;

		if (errCode != 200)
		{
			break;
		}

		//��д����
		response += generateTimeLine();

		/*
			���ɻỰ��Session
		*/

		tmp = sessGenerator.getSessionID();

		unsigned long session = stol(tmp);

		response = response + "Session: " + tmp + "\r\n";

		/*
			���Ƚ����ͻ�������Ĵ��ͷ�ʽ�Ͷ˿ںţ�����ͻ���������Ϣ��Ȼ��ط��Ự��
			���и��ؼ��ǡ���
			��������Ϣ��RTP�����õģ��������ڲ����ˣ��ȴ��Ű�
			Ȼ��������Ϊ����֣����и��Ʒ��ؼ���
		*/

		PerClientData clientData;

		//�������ͻ��˶˿�
		clientData.streamingPort = stoi(msg.substr(msg.rfind("="), msg.rfind("-")));

		//ȷ���Ƿ���UDP
		if (msg.find("UDP") != string::npos)
		{
			clientData.enableUDP = true;
		}
		else
		{
			clientData.enableUDP = false;
		}

		//�ѿͻ��˼����б���
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
			���Ƚ���Session���ж��Ƿ�Ϸ�
			���Ϸ�������Ӧ����454,Session Not Found
		*/

		unsigned long session = stol(msg.substr(msg.find("Session:") + 8));

		if (clientManager.searchClient(session) != 0)
		{
			/*
				TODO���Ự����ȷ�������Ӧ�õ��ò�����������

				�����ڵ�����£�PLAY����ĺ���Ϊ��ʹ�÷�����ܹ���ÿͻ��˽��в�����Ϊ����Ӧ������ͼ
				��������TMDɶ������д������ʵ��SETUP֮��Ϳ��Խ����ˡ���
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

		//��д����
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
			�����뵽һ�ִ�����������㲻��TEARDOWNһ�������ڵĻỰ��
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

		//�Ƴ��ͻ�����Ϣ
		clientManager.removeClient(session);

		break;
	}
	default:
	{
		//�ܽ�������˵������������������ء���405,Method Not Allowed

		errCode = 405;

		response = generateCMDLine(errCode) + seqNum;

		break;
	}
	}

	/*
		����֮ǰ��Щ�ط�ֱ�ӿ��󻺴������
		�������Ҫ����һ������ȥ������ĺ�׺�ո�
	*/
	response = response.substr(0, response.rfind('\n'));

	return response;
}

//����ʱ�䣺"Date: 23 Jan 1997 15:35:06 GMT"
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

//���������У���һ�У����汾��+������+������Ϣ
string rtspHandler::generateCMDLine(int errCode)
{
	string msg;

	msg = "RTSP/" + rtspVersion + " " + to_string(errCode) + " " + getErrMsg(errCode) + "\r\n";

	return msg;
}