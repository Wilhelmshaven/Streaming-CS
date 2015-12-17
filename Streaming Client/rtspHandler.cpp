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
	streamingPort = 54236;
	enableUDP = false;
}

//���ش�����Ϣ
string rtspHandler::getErrMsg(int code)
{
	return errHandler.getErrMsg(code);
}

//�ı���ý��������������ͺ��ˣ��Ƿ�UDP�Լ��˿ڣ�
bool rtspHandler::setHandler(WORD rtspVer, int port, bool enableUDP)
{
	if(session.empty())rtspVersion = rtspVer;
	streamingPort = port;
	this->enableUDP = enableUDP;

	return true;
}

//����rtsp�������ĵ�ǰ��Ϣ
string rtspHandler::getHandlerInfo()
{
	string msg;
	sprintf_s((char *)msg.data(), BUF_SIZE, "RTSP�汾��%d.%d\r\n�Ự�ţ�session����%s\r\n�˿ںţ�%d\r\n���䷽ʽ��%s\r\n",
		rtspVersion >> 8 & 0xff, rtspVersion & 0xff,
		session,
		streamingPort,
		enableUDP ? "UDP" : "TCP");
	return msg;
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