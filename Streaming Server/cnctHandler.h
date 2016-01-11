#pragma once�ڡ�
#include "CommonHeaders.h"

class cnctHandler
{
public:
	WSADATA wsaData;

	//һϵ�з�����Ϣ�ĺ���
	static cnctHandler *getInstance();           //���ص���

	//���÷���������
	void srvConfig(int port);

	//������������ֱ�ӽ��е�������
	void startServer();

	//�߳����
	vector<HANDLE> workerThread;             //�������̣߳�����ΪCPU����*2
	static UINT workerThreadFunc();          //�̴߳�����

	~cnctHandler();                   //��Ҫ������������Ҫд������

private:
	static cnctHandler *instance;              //����
	cnctHandler();                             //���캯��

	SOCKET srvSocket;                          //Ψһ��SOCKET�������Ҹ�

	//��ֹ���������Լ���ֵ
	cnctHandler(const cnctHandler &);
	cnctHandler & operator = (const cnctHandler &);

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
	string srvPort;            //ָ���������˿�
	SYSTEM_INFO sysInfo;       //�洢ϵͳ��Ϣ�Ľṹ	
	SOCKADDR_IN srvAddr;       //��������ַ�ṹ

	HANDLE completionPort;     //��ɶ˿�

	//Function
	void getSystemInfo();          //��ȡϵͳ��Ϣ������Ҫ��CPU�������Ա㴴��Worker�̣߳�
	void buildThread();            //����CPU���������������߳�
	bool isSocketAlive(SOCKET s);

	
};