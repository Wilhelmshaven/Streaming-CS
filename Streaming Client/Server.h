#include "CommonHeaders.h"

//����������ṹ����
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
//To do list�����������������ܣ�����У��
class Server
{
public:
	WSADATA wsaData;

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

	//���ص�ǰ��Ϣ��
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