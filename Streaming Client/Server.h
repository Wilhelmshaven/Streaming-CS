#include "CommonHeaders.h"

//����������ṹ����
class serverList
{
public:
	string getConfigByIndex(int index);   //��ȡ��Ӧ����

	string srvArgs[6];      //��������������ϸ�Ĳ����������ϵĳ�����

	serverList *prev;
	serverList *next;

	void getLabelMsg(string name, string buf);    //��д����	

};

//Server�࣬������ȡ�����ļ�������Winsock���ӣ�������
class Server
{
public:
	WSADATA wsaData;

	enum
	{
		reserve = 0,       //Ĭ��ֵ��������
		srvType = 1,       //����������
		protocol = 2,      //��ַͷ
		hostName = 3,      //����
		port = 4,          //�˿�
		displayRoute = 5,  //���ŵ�ַ
	};

	static Server *getInstance()
	{
		return instance;
	}

	int connectServer();                 //���ӷ��������ɹ�����0

	//����SOCKETʵ��
	SOCKET getSocket()
	{
		return srvSocket;
	}           

	//Test Code
	void setSocket(SOCKET in)
	{
		srvSocket = in;
	}

	serverList* getServerInfo()
	{
		return mySrvList->next;
	}

	string getDisplayAddr()
	{
		return displayAddr;
	}

protected:
	static Server *instance;              //����
	Server(string file);                  //���캯��

	serverList *mySrvList;                //������������Ƕ��

	SOCKET srvSocket;                     //�����Ҹ�

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
	string fileName;           //�����ļ���

	// ��ǰ����ʹ�õķ�������Ϣ
	sockaddr_in srvAddr;       //��ַ�ṹ��ipv4/6��IP���˿ڣ�
	string displayAddr;        //�����Ĳ��ŵ�ַ��eg. http://www.rayion.com/desktop

	bool readFile();  // ��ȡ�����ļ�
};