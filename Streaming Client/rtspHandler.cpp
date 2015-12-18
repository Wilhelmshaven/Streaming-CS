#include "rtspHandler.h"

/*----------------------RTSP���Ӵ�������RTSP ��������Ϣ��ѯ---------------------*/
//ֱ�ӳ�ʼ��
rtspHandler *rtspHandler::instance = new rtspHandler();

//һϵ�з��������Ϣ�ĺ���
rtspHandler* rtspHandler::getInstance()
{
	return instance;
}

//RTSP���������캯��
rtspHandler::rtspHandler()
{
	seqNum = 1;                   //��ʼ�����к�Ϊ1
	rtspVersion = MAKEWORD(1, 0);
	streamingPort = 8554;
	enableUDP = false;
}

//���ش�����Ϣ
string rtspHandler::getErrMsg(int code)
{
	return errHandler.getErrMsg(code);
}

//�ı���ý��������������ͺ��ˣ���ַ���Ƿ�UDP�Լ��˿ڣ�
bool rtspHandler::setHandler(string URI, WORD rtspVer, int port, bool enableUDP)
{
	this->URI = URI;

	sprintf_s((char *)rtspVersion.data(), 4, "%d.%d", rtspVer >> 8 & 0xff, rtspVer & 0xff);

	//ֻ����ż���˿ں�
	if (port % 2 != 0)streamingPort = (port + 1) % 65535;
	else streamingPort = port;

	this->enableUDP = enableUDP;

	return true;
}

//����rtsp�������ĵ�ǰ��Ϣ
string rtspHandler::getHandlerInfo()
{
	string msg;
	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP�汾��%s\r\n�Ự�ţ�session����%s\r\n�˿ںţ�%d\r\n���䷽ʽ��%s\r\n",
		rtspVersion,
		session,
		streamingPort,
		enableUDP ? "UDP" : "TCP");
	return msg;
}

//rtsp������������õ��ĺ��پ�һ���������ˣ�
string rtspHandler::encodeMsg(int method)
{
	string msg;

	//����ֻ�����̴�����ѡ���
	string reqLine;
	sprintf_s((char *)reqLine.data(), BUF_SIZE, " %s RTSP/%s\r\nCSeq:%d\r\n", URI.c_str(), rtspVersion, seqNum);

	switch (method)
	{
	case DESCRIBE:
	{
		/*Example Message*/
		/*
		DESCRIBE rtsp://example.com/test.mp3 RTSP/1.0
		CSeq: 302
		*/
		msg = "DESCRIBE" + reqLine;
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
			session);
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
			session);
		msg = "GET_PARAMETER" + reqLine + msg;
		break;
	}
	default:
		break;
	}

	++seqNum;          //��Ҫ�����к�������
	return msg;
}

//rtsp���������
int rtspHandler::decodeMsg(string msg)
{
	int errCode;           //�������
	int sequence;          //���кţ����˶ԣ������Ҫ
	string buf;

	//Ϊ�˽�����ϣ��ǲ��ǲ�Ҫȥ���ǵ����Ƿ���ʲô��Ϣ�Ļظ�
	//�о����ܻ���Ҫ���ǰɣ�����������ʱ�Ͳ�������
	//�������ﲢ���ܷ��ͣ������֮��һ���ͷ��˰���˭֪���ǲ��Ǳ�����
	//��Ŀǰ��Ӧ�ö��ԣ���ʵֻ��Ҫ����������һ���Ǵ�����루��ʵ������Ϣ����Ҳ���˸������ò����һ���ǻỰ��

	//��ȡ������
	buf = "RTSP/" + rtspVersion;
	buf = msg.substr(msg.find(buf) + 1, msg.find(buf) + 4);
	errCode = atoi(buf.c_str());

	//��ȡ���к�
	buf = msg.substr(msg.find("CSeq: "));
	buf = buf.substr(0, buf.find("/r"));
	sequence = atoi(buf.c_str());

	//��ȡ�Ự�ţ�����������ĻỰ��Ϊ����д�룬����У�飩
	if (session.empty())
	{
		session = msg.substr(msg.find("Session: "), 8);
	}
	else
	{
		//string ssnTmp = msg.substr(msg.find("Session: "), 8);
		//if...
	}

	return errCode;
}

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
			msg = buf.substr(commaPos + 1, buf.length() - code.length());
			errCode = atoi(code.c_str());

			errCodeList.insert(make_pair(errCode, msg));
		}
	}
}

//����ض�Ӧ��Ϣ
string rtspErrHandler::getErrMsg(int code)
{
	string msg = errCodeList.find(code)->second;
	if (msg.empty())msg = "Invalid Error Code";

	return msg;
}