#pragma once
#include "CommonHeaders.h"

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

	static clientManager *getInstance();

	//����ͻ���
	bool addClient(unsigned long session, SOCKET socket, unsigned int port, bool enableUDP);

	//��ѯ�ͻ����Ƿ����
	bool searchClient(unsigned long session);
	bool searchClient(SOCKET socket);

	//����Ự�ţ���ÿͻ�����Ϣ
	bool getClientInfo(unsigned long session, SOCKET &socket, unsigned int &port, bool &enableUDP);

	//�Ƴ��ͻ���
	bool removeClient(unsigned long session);

	map<SOCKET, unsigned long> socToSessList;

private:

	/*
		Ϊÿһ���Ự�洢��Ӧ���ݵĽṹ��
		�������׽��֣��˿ںţ�����ģʽ��TCP/UDP��
	*/
	typedef struct PerClientData
	{
		SOCKET socket;

		unsigned int streamingPort;

		bool enableUDP;
	};

	//Key-Value MAP
	map<unsigned long, PerClientData> clientList;
	

	/*
		Singleton
	*/

	clientManager();
	static clientManager *instance;

	clientManager(const clientManager &);
	clientManager & operator = (const clientManager &);

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