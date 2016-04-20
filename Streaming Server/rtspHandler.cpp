/*--Author������--*/

#include "rtspHandler.h"

//���ؿͻ��˹�����
#include "clientManager.h"

//���ش�����ģ��
#include "errHandler.h"

//һ��Session��ͻ��˲����Ķ�Ӧ�� 
clientManager *clientList = clientManager::getInstance();

namespace rtspNS
{
	HANDLE heStartPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::play);
	HANDLE heStopPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::stop);
	HANDLE hePausePlay = CreateEvent(NULL, TRUE, FALSE, syncManager::pause);
};

using namespace rtspNS;

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

	const long daysInMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

	time += (sysTime.wYear - 1900) * 365;
	for (int i = 1900; i < sysTime.wYear; ++i)
	{
		if (i % 4 == 0 && i != 1900)++time;
	}

	for (int i = 1; i < sysTime.wMonth; ++i)time += daysInMonth[i];

	time += sysTime.wDay - 1;

	time *= 86400;
	time += (sysTime.wHour - 1) * 3600;
	time += (sysTime.wMinute - 1) * 60;
	time += sysTime.wSecond;

	//��ֹ���˶�㣬��ϵͳʱ����ñȹ涨ʱ��㻹ǰ����
	if (time < 0)time = 0;

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

	if (time < 0)time = 0;

	return time;
}

/*----------------------SDP������---------------------*/
sdpEncoder::sdpEncoder()
{
	
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
	proVer = "v=0\r\n";

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
	session = 1;
	time = -1;
}

/*
	�Ự�����ɺ���
	����ֱ��ʹ��NTPʱ�������Ϊ�Ự��
*/
string sessionGenerator::getSessionID()
{
	unsigned long newTimeStamp = ntpTime.getNTPTimeUnix();

	string newSession;
	newSession.resize(9);

	if (time != newTimeStamp)
	{
		time = newTimeStamp;

		//Ҫ���н���ת������Ȼ�ǵ�sprintf_s�Ľ��ࣿ
		sprintf_s((char *)newSession.data(), 9, "%X", time);
	}
	else
	{
		if (session == time)session = 1;

		sprintf_s((char *)newSession.data(), 9, "%X", session);

		++session;
	}

	return newSession;
}

/*----------------------RTSP��Ϣ������---------------------*/

rtspHandler *rtspHandler::instance = new rtspHandler;

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
	srvURI = "http://localhost:8554";

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

rtspHandler* rtspHandler::getInstance()
{
	return instance;
}

/*
	���������÷��������ԣ�����ֵΪ0
	Ŀǰֻ��Ҫ���ö˿ں�
*/
void rtspHandler::srvConfig(string URI, unsigned int srvPort)
{
	srvURI = URI;

	this->srvPort = srvPort;
}

/*
	����������rtsp�������ĵ�ǰ��Ϣ������RTSP�汾��[�汾��]��
	�磺RTSP�汾��1.0
*/
string rtspHandler::getHandlerInfo()
{
	string msg;

	msg = "RTSP�汾��" + rtspVersion + "\r\n";

	return msg;
}

/*
	����뷽��

	160303������ȷ���߼���û���κ�����
*/
string rtspHandler::msgCodec(SOCKET socket, string msg)
{
	//���շ��صĴ�
	string response;

	/*
		����Ľṹ���Թ���Ϊ��

		Request URI rtspVersion\r\n
		CSeq: seqNum\r\n
		PADDING_MSG\r\n
	*/
	string request, URI, rtspVersion, seqNum, paddingMsg;

	//����Ĵ����룬��ʼ��Ϊ200��OK
	int errCode = 200;

	/*
		1.���ָ�����תΪ���ֱ��
		��ͷ����һ���ո�伴Ϊ����
	*/

	request = extractString(msg);

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

	URI = extractString(msg);

	rtspVersion = extractString(msg, true);

	/*
		3. ������к�
		ע�⣺��������ʵ��������Ҫ������Ϊ���٣�ֻ��Ҫ����һ��ԭ�����ؼ���
		�������и���
	*/

	seqNum = extractString(msg, true) + "\r\n";

	/*
		4. ���PADDING��Ϣ������ָ��������������Խ������
		!!Ҳ����˵����������һ����
	*/

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
		string tmp = "Public: ";

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
		string sdp = sdpHandler.encodeMsg();

		response = response + "Content-Type: application/sdp\r\nContent-Length: " + to_string(sdp.length()) + "\r\n\r\n" + sdp;

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
		string sess = sessGenerator.getSessionID();

		unsigned long session = stoul(sess, nullptr, 16);

		response = response + "Session: " + sess + "\r\n";

		/*
			���Ƚ����ͻ�������Ĵ��ͷ�ʽ�Ͷ˿ںţ�����ͻ���������Ϣ��Ȼ��ط��Ự��
			���и��ؼ��ǡ���
			��������Ϣ��RTP�����õģ��������ڲ����ˣ��ȴ��Ű�
			Ȼ��������Ϊ����֣����и��Ʒ��ؼ���
		*/

		//�������ͻ��˶˿�
		int streamingPort = stoi(paddingMsg.substr(paddingMsg.rfind("=") + 1, paddingMsg.rfind("-") - paddingMsg.rfind("=") - 1));

		//ȷ���Ƿ���UDP
		bool enableUDP = false;		
		if (paddingMsg.find("UDP") != string::npos)
		{
			enableUDP = true;
		}

		/*
			!!
			�ѿͻ��˼����б���
			��һ��Ҳ����Ҫ������Ҫʵ�֣���ӳ����play-teardown�е�play����
		*/
		clientList->addClient(session, socket, streamingPort, enableUDP);

		paddingMsg.pop_back();
		paddingMsg.pop_back();
		response = response + paddingMsg + "; server_port: " + to_string(srvPort) + "-" + to_string(srvPort + 1) + "\r\n";

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
		unsigned long session = stoul(extractSession(paddingMsg), nullptr, 16);

		if (clientList->searchClient(session))
		{
			/*
				�Ự����ȷ�������Ӧ�õ��ò�����������
			*/	

			ResetEvent(hePausePlay);

			SetEvent(heStartPlay);
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
	case PAUSE:
	{
		/*Example Message*/
		/*
			C->S:
			PAUSE rtsp://example.com/fizzle/foo RTSP/1.0
            CSeq: 834
            Session: 12345678

			S->C: 
			RTSP/1.0 200 OK
            CSeq: 834
            Date: 23 Jan 1997 15:35:06 GMT
		*/

		/*
			���Ƚ���Session���ж��Ƿ�Ϸ�
			���Ϸ�������Ӧ����454,Session Not Found
		*/
		unsigned long session = stoul(extractSession(paddingMsg), nullptr, 16);

		if (clientList->searchClient(session))
		{
			/*
				�Ự����ȷ�������Ӧ�õ��ò�������ͣ��
			*/

			ResetEvent(heStartPlay);

			SetEvent(hePausePlay);
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
		unsigned long session = stoul(extractSession(paddingMsg), nullptr, 16);

		if (clientList->searchClient(session))
		{
			/*
				֪ͨ������ģ��ֹͣ����
			*/
			
			SetEvent(heStopPlay);

			//�Ƴ��ͻ�����Ϣ
			clientList->removeClient(session);
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

		break;
	}
	case GET_PARAMETER:
	{
		/*Example Message*/
		/*
			C->S:
			GET_PARAMETER rtsp://example.com/fizzle/foo RTSP/1.0
			CSeq: 892
			Session: 12345678

			S->C:
			RTSP/1.0 200 OK
			CSeq: 892
		*/
		unsigned long session = stoul(extractSession(paddingMsg), nullptr, 16);

		if (clientList->searchClient(session))
		{
			//TODO��Nothing to do...
		}
		else
		{
			errCode = 454;
		}

		response = generateCMDLine(errCode) + seqNum;

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
	response = response.substr(0, response.rfind('\n')) + "\r\n";

	return response;
}

//����ʱ�䣺"Date: 23 Jan 1997 15:35:06 GMT"
string rtspHandler::generateTimeLine()
{
	const string engMonth[12] = {" Jan "," Feb ", " Mar ", " Apr ", " May ", " Jun ", " Jul ", " Aug ", " Sep ", " Oct ", " Nov ", " Dec "};

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);

	string msg = "Date: ";

	msg = msg + to_string(sysTime.wDay) + engMonth[sysTime.wMonth - 1] + to_string(sysTime.wYear);

	string clock;
	clock.resize(9);
	sprintf_s((char *)clock.data(), 9, "%02d:%02d:%02d", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	msg = msg + " " + clock + "GMT\r\n";

	return msg;
}

//���������У���һ�У����汾��+������+������Ϣ
string rtspHandler::generateCMDLine(int errCode)
{
	rtspErrHandler *errHandler = rtspErrHandler::getInstance();

	string msg;

	msg = "RTSP/" + rtspVersion + " " + to_string(errCode) + " " + errHandler->getErrMsg(errCode) + "\r\n";

	return msg;
}

//��ȡ��һ���ַ���
string rtspHandler::extractString(string &msg, bool newLine)
{
	/*
		ѭ�������󷨣���ȡ�����һ���ֶ�->ȥ����һ���ֶ�

		info = msg.substr(0,msg.find(�ո����);
		msg=msg.substr(info.length()+1����2);
	*/

	string mark = " ";

	if (newLine)mark = "\r";

	string extract = msg.substr(0, msg.find(mark));

	if (newLine)
	{
		msg = msg.substr(extract.length() + 2);
	}
	else
	{
		msg = msg.substr(extract.length() + 1);
	}

	return extract;
}

string rtspHandler::extractSession(string msg)
{
	string tmp = msg.substr(msg.find("Session") + 8);

	tmp = tmp.substr(0, tmp.find("\r"));

	return tmp;
}