#include "CommonHeaders.h"

//����������ṹ���壬˫������
class serverList
{
public:
	string getCfgByIndex(int index);   //��ȡ��Ӧ����

	string srvArgs[ServerArgc];      //��������������ϸ�Ĳ�������������ServerInfoDefine

	serverList *prev;
	serverList *next;

	void getLabelMsg(string name, string buf);    //��д����	
};

//Server�࣬������ȡ�����ļ�������Winsock���ӣ�������
class Server
{
public:
	WSADATA wsaData;

	//һϵ�з�����Ϣ�ĺ���
	static Server *getInstance();     //���ط�����ʵ��
	SOCKET getSocket();               //����SOCKETʵ��
	serverList* getServerInfo();      //���ط���������ͷָ��
	string getDisplayAddr();          //����������ַ
	int getStreamPort();              //���ض˿ں�

	int connectServer();              //���ӷ��������ɹ�����0

	~Server();                        //��Ҫ������������Ҫд������

private:
	static Server *instance;              //����
	Server(string file);                  //���캯��

	serverList *mySrvList;                //������������Ƕ��

	SOCKET srvSocket;                     //Ψһ��SOCKET�������Ҹ�

	//��ֹ���������Լ���ֵ
	Server(const Server &);
	Server & operator = (const Server &);

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

	// VARs
	string fileName;           //�����ļ����ļ���

	// ��ǰ����ʹ�õķ�������Ϣ
	sockaddr_in srvAddr;       //��ַ�ṹ��ipv4/6��IP���˿ڣ�
	string displayAddr;        //�����Ĳ��ŵ�ַ��eg. http://www.rayion.com/desktop

	bool readFile();  // ��ȡ�����ļ�
};