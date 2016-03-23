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
	Ϊ�˱�֤�Ự��Ψһ�����ͬһ���ڴ����˶�����ӣ�����������ʹ�ô�1��ʼ�ĻỰ��
*/
class sessionGenerator
{
public:

	sessionGenerator();

	string getSessionID();

private:

	unsigned long session;

	unsigned long time;
	
	NTPTimeGenerator ntpTime;
};

/*
	�ͻ����б���

	ʹ�ã�

	bool addClient(unsigned long session, SOCKET socket, int port, bool enableUDP)������ͻ���

	bool searchClient(unsigned long session)����ѯ�ͻ����Ƿ����

	bool getClientInfo(unsigned long session, SOCKET &socket, int &port, bool &enableUDP)����ÿͻ�����Ϣ

	bool removeClient(unsigned long session)���Ƴ��ͻ���
*/
class clientManager
{
public: 

	//����ͻ���
	bool addClient(unsigned long session, SOCKET socket, int port, bool enableUDP);   
	
	//��ѯ�ͻ����Ƿ����
	bool searchClient(unsigned long session); 
	
	//��ÿͻ�����Ϣ
	bool getClientInfo(unsigned long session, SOCKET &socket, int &port, bool &enableUDP);

	//�Ƴ��ͻ���
	bool removeClient(unsigned long session);                          
	 
	clientManager();

private:

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

	//Key-Value MAP
	map<unsigned long, PerClientData> clientList;
};

/*
	RTSP��Ϣ������

	ʹ�ã�

	void srvConfig(string URI = "", unsigned int srvPort = 8554)�����������ú�����Ŀǰֻ����������ʹ�õĶ˿ں�

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

	//���÷���������
	void srvConfig(string URI = "http://localhost:8554", unsigned int srvPort = 8554);

	//�����һ�壬�����������Ϣ�����ر���õĴ𸴣���������
	string msgCodec(SOCKET socket, string msg);

	//���ش����������ʾ����Ϣ
	string getErrMsg(int code);      

	//��ȡrtsp�������������Ϣ
	string getHandlerInfo();         

	//���ڣ�ȡ��������ĻỰ������RTPģ�鷢��
	bool getWaitingSession(unsigned long &session, SOCKET &socket, bool &enableUDP);

private:

	typedef struct PerClientData
	{
		unsigned long session;

		SOCKET socket;

		int streamingPort;

		bool enableUDP;
	};

	//�ȴ�RTP����ĻỰ����
	queue<PerClientData> sendQueue, stopQueue;

	//һ��Session��ͻ��˲����Ķ�Ӧ�� 
	clientManager clientList;

	//������RTSP����
	vector<string> rtspMethod;                 
	
	//���õķ�����RTSP�������������RTSP���������Ӧ
	vector<bool> availableMethod;              	

	/*
		��������Ϣ����ʵ������ý���������Ϣ�ˣ�
	*/

	//��ý���ַ������Ŀǰû���ϣ���֪����ɶ��
	string srvURI;   

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

	//���������һ��
	string generateCMDLine(int errCode);

	//����ʱ����һ��
	string generateTimeLine();

	//��ȡ��һ���ַ���
	string extractString(string &msg, bool newLine = false);

	//��ȡ���Ự��
	string extractSession(string msg);
	
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