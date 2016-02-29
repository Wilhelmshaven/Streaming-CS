/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	RTSP������Ϣ������
	ʹ�ã�
	string getErrMsg(int code)����������룬���ش�����Ϣ
*/
class rtspErrHandler
{
public:
	rtspErrHandler();

	//���������Ϣ����������룬���ش�����Ϣ�ַ���
	string getErrMsg(int code);      

private:
	/*
		rtsp������Ϣ�ļ����ļ���ַ����Ե�ַ��
		TODO�����޸����������ָ��λ��
	*/
	const string rtspErrFile = "config/static/rtspErrCodeList.csv";       

	string settingFile;
	void buildErrList();             //�����������ӱ��������ļ������ڻ������ݲ��ϸ�ʽ�ˣ�ֻ��Ϊ�˼��ٴ��������ѣ�Ҳ����

	map<int, string> errCodeList;    //�������ӱ�
};

/*
	NTPʱ���ȡ��
	ʹ�ã�
	unsigned long getNTPTime()������NTPʱ�䣨��1900.1.1 00:00��
	unsigned long getNTPTimeUnix()������UNIX��NTPʱ�䣨��1970.1.1 00:00��
*/
class NTPTime
{
public:
	unsigned long getNTPTime();
	unsigned long getNTPTimeUnix();
};

/*
	SDP��������SDPЭ��ϸ�������RFC2327�ĵ���http://tools.ietf.org/html/rfc2327

	string encodeMsg()�����룬���ر���õ�SDP��Ϣ

	TODO��̫�࡭��
*/
class sdpEncoder
{
public:
	string encodeMsg();

	sdpEncoder();

private:
	NTPTime ntpTime;
	string sdpMsg;

	//--Session description
	//Require
	string proVer;
	string owner;
	string sessionName;
	//Option
	string sessionInfo;
	string URI;
	string emailAddr;
	string phoneNum;
	string cnctInfo;
	string bandwidthInfo;
	string timeZoneAdjust;
	string encryptKey;
	string moreSessionAttr;

	//Time description
	string timeActive;     //require
	string repeatTime;     //option

	//Media description
	string mediaNameAndAddr;     //require
	string mediaTitle;
	//string cnctInfo;             //optional if included at session-level
	//string bandwidthInfo;
	//string encryptKey;
	//string moreSessionAttr;
};

/*
	һ�����Session��������

	string getSessionID()������һ���Ự��
	
	ps.
	�������Ҿ���ֱ����ʱ����ͺ��ˡ�������һ��0xFFFFFFFF���㹻��
	�⵱Ȼ�����⣬�����뿴ʵ�ֲ���
*/
class sessionGenerator
{
public:
	sessionGenerator();

	string getSessionID();

private:
	string session;
	
	NTPTime ntpTime;
};

//Ϊÿһ���ͻ��˴洢��Ӧ����
typedef struct PerClientData
{
	int streamingPort;
	bool enableUDP;
	int srvPort;
};

/*
	�ͻ����б���

	������
	int addClient(unsigned long session, PerClientData clientInfo)������ͻ��ˣ�����Ự�źͿͻ�����Ϣ�ṹ��

	unsigned long searchClient(unsigned long session)����ѯ�ͻ����Ƿ���ڣ�����Ự�ţ����ػỰ�Ż�0�������ڣ�

	PerClientData getClientInfo(unsigned long session)����ÿͻ�����Ϣ������Ự�ţ����ؿͻ�����Ϣ�ṹ��

	int removeClient(unsigned long Session)���Ƴ��ͻ��ˣ�����Ự�ţ�����0
*/
class clientList
{
public: 

	//����ͻ���
	int addClient(unsigned long session, PerClientData clientInfo);   
	
	//��ѯ�ͻ����Ƿ����
	unsigned long searchClient(unsigned long session); 
	
	//��ÿͻ�����Ϣ
	PerClientData getClientInfo(unsigned long session);                

	//�Ƴ��ͻ���
	int removeClient(unsigned long Session);                          
	 
	clientList();

private:
	map<unsigned long, PerClientData> client;
};

/*
	RTSP��Ϣ������

	������
	string getErrMsg(int code)��������Ϣ����������������룬���ش����������ʾ����Ϣ

	string getHandlerInfo()����ȡrtsp�������������Ϣ��������Ϣ�ַ���

	string msgCodec(string msg)�������һ�庯��������ͻ������Ĵ�������Ϣ�����ر���õĴ���Ϣ

	int srvConfig(unsigned int srvPort)�����������ú�����Ŀǰֻ����������ʹ�õĶ˿ں�

	ps.
	��������������������ͻ������űȽϴ������
	ÿ���ͻ��˵����󶼲�һ��������Ҫ�����һ�壨�������ɣ���Ȼ�Ҿ��û��ǿ��Բ𿪵ģ�

	TODO��̫��TODO�ˡ���
*/
class rtspHandler
{
public:
	static rtspHandler *getInstance();

	string getErrMsg(int code);      //���ش����������ʾ����Ϣ
	string getHandlerInfo();         //��ȡrtsp�������������Ϣ

	//ֻ�ܱ���룬��������
	string msgCodec(string msg);     //�����һ�壬�����������Ϣ�����ر���õĴ�

	//���÷���������
	int srvConfig(unsigned int srvPort);

private:
	string URI;                                //��ý���ַ
	string rtspVersion;                        //RTSP�汾
	unsigned int srvPort;                      //�������˿�

	rtspErrHandler errHandler;                 //������Ϣ������
	sdpEncoder sdpHandler;                     //SDP������

	vector<string> rtspMethod;                 //������rtsp����
	vector<bool> availableMethod;              //���õķ�����rtsp����

	clientList clientManager;                  //һ��Session��ͻ��˲����Ķ�Ӧ��  
	sessionGenerator sessGenerator;            //�Ự��������

	/*
		����ģʽ
	*/
	static rtspHandler *instance;              //����
	rtspHandler();                             //���캯��

	//��ֹ���������Լ���ֵ
	rtspHandler(const rtspHandler &);
	rtspHandler & operator = (const rtspHandler &);

	//��������
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo Garbo;
};