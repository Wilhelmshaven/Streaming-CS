/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	RTSP������Ϣ������

	ʹ�ã�

	rtspErrHandler(string filePath = "config/static/rtspErrCodeList.csv")������ѡ��ָ���������Ե�ַ����������

	string getErrMsg(int code)����������룬���ش�����Ϣ
*/
class rtspErrHandler
{
public:

	rtspErrHandler(string filePath = "config/static/rtspErrCodeList.csv");

	//���������Ϣ����������룬���ش�����Ϣ�ַ���
	string getErrMsg(int code);      

private:

	//rtsp������Ϣ�ļ����ļ���ַ����Ե�ַ��  
	string settingFile;

	//�����������ӱ���ʱ�Ͳ��������ݲ��ϸ�ʽ��
	void buildErrList();             

	//�������ӱ�Key-Value Map
	map<int, string> errCodeList;    
};

/*
	NTPʱ���ȡ��

	ʹ�ã�

	unsigned long getNTPTime()������NTPʱ�䣨��1900.1.1 00:00��

	unsigned long getNTPTimeUnix()������UNIX��NTPʱ�䣨��1970.1.1 00:00��
*/
class NTPTimeGenerator
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

	NTPTimeGenerator ntpTime;

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
	
	NTPTimeGenerator ntpTime;
};

/*
	Ϊÿһ���Ự�洢��Ӧ���ݵĽṹ��
	�������׽��֣��˿ںţ�����ģʽ��TCP/UDP��
*/
typedef struct PerClientData
{
	SOCKET socket;

	int streamingPort;

	bool enableUDP;
};

/*
	�ͻ����б���

	ʹ�ã�

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
	int removeClient(unsigned long session);                          
	 
	clientList();

private:

	//Key-Value MAP
	map<unsigned long, PerClientData> client;
};

/*
	RTSP��Ϣ������

	ʹ�ã�

	int srvConfig(string URI = "", unsigned int srvPort = 8554)�����������ú�����Ŀǰֻ����������ʹ�õĶ˿ں�

	string msgCodec(string msg)�������һ�庯��������ͻ������Ĵ�������Ϣ�����ر���õĴ���Ϣ

	string getHandlerInfo()����ȡrtsp�������������Ϣ��������Ϣ�ַ���

	string getErrMsg(int code)��������Ϣ����������������룬���ش����������ʾ����Ϣ

	ps.
	��������������������ͻ������űȽϴ������
	ÿ���ͻ��˵����󶼲�һ��������Ҫ�����һ�壨�������ɣ���Ȼ�Ҿ��û��ǿ��Բ𿪵ģ�
	Ҳ���Կ����ù���ģʽ����

	TODO��̫��TODO�ˡ���
*/
class rtspHandler
{
public:

	static rtspHandler *getInstance();

	//�����һ�壬�����������Ϣ�����ر���õĴ𸴣���������
	string msgCodec(SOCKET socket, string msg);

	//���ش����������ʾ����Ϣ
	string getErrMsg(int code);      

	//��ȡrtsp�������������Ϣ
	string getHandlerInfo();         

	//���÷���������
	int srvConfig(string URI = "", unsigned int srvPort = 8554);

	unsigned long getWaitingSession();

	SOCKET getWaitingSocket(unsigned long session);

private:

	//�ȴ�RTP����ĻỰ����
	queue<unsigned long> waitingQueue;

	//һ��Session��ͻ��˲����Ķ�Ӧ�� 
	clientList clientManager;

	//������RTSP����
	vector<string> rtspMethod;                 
	
	//���õķ�����RTSP�������������RTSP���������Ӧ
	vector<bool> availableMethod;              	

	/*
		��������Ϣ����ʵ������ý���������Ϣ�ˣ�
	*/

	//��ý���ַ������Ŀǰû���ϣ���֪����ɶ��
	string URI;   

	//RTSP�汾
	string rtspVersion;  

	//�������˿�
	unsigned int srvPort;                      

	/*
		��������Сģ��
	*/

	//������Ϣ������
	rtspErrHandler errHandler;  

	//SDP������
	sdpEncoder sdpHandler;                             

	//�Ự��������
	sessionGenerator sessGenerator;            

	/*
		��װ����������ظ��Ĵ����/����
	*/

	string generateCMDLine(int errCode);

	string generateTimeLine();

	/*
		����ģʽ
	*/

	rtspHandler();

	static rtspHandler *instance;                  

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

//��ý������ģ�飺������µĲ���/ֹͣ����������RTPģ�����߻Ự��
HANDLE hsPlaySession = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);
HANDLE hsStopSession = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);