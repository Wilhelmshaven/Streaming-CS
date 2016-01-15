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

			commaPos = buf.find(',');
			code = buf.substr(0, commaPos);
			errCode = stoi(code, nullptr, 10);

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

//һϵ�з��������Ϣ�ĺ���
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

	//---------------����ָ���������Խ������----------------//
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

		//���ڣ����ͣ�����

		//������SDP��Ϣ

		break;
	}
	default:
		break;
	}

	return response;
}

