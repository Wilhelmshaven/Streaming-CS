/*--Author������--*/
#pragma once
#include "CommonHeaders.h"

/*
	Ϊÿһ���Ự�洢��Ӧ���ݵĽṹ��
	�������׽��֣��˿ںţ�����ģʽ��TCP/UDP��
*/
typedef struct PerClientData
{
	SOCKET socket;

	unsigned int streamingPort;

	bool enableUDP;

	//ͼ�����ű��ʣ����������ͷ��ƣ�
	double scaleFactor;

	//�Ƿ񲥷�
	bool play;

	//֡�ʣ�ʵ�����Ƿ���Ƶ�ʣ�����ͷ��֡���ǲ���ģ�
	unsigned int frameRate;
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

	/*
		Ϊ����ͷ���ӵ��������
	*/

	//��������ʼ������
	map<unsigned long, PerClientData>::iterator getIteratorStart();

	//�������յ������
	map<unsigned long, PerClientData>::iterator getIteratorEnd();

	//�ı��Ƿ񲥷�����
	bool changePlayFactor(SOCKET index, bool play);

	//�ı������ʣ�ģ�ⲻͬ�ֱ��ʴ��䣩
	bool changePlayFactor(SOCKET index, double scaleFactor);

	//�ı�֡�ʣ�����ƫ������
	bool changePlayFactor(SOCKET index, int offset);

private:

	//Key-Value MAP
	map<unsigned long, PerClientData> clientList;
	
	map<SOCKET, unsigned long> socToSessList;

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