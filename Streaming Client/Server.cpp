#include "Server.h"

//ֱ�ӳ�ʼ��
Server *Server::instance = new Server(srvSettingFile);

//һϵ�з��������Ϣ�ĺ���
Server* Server::getInstance()
{
	return instance;
}

SOCKET Server::getSocket()
{
	return srvSocket;
}

serverList* Server::getServerInfo()
{
	return mySrvList;
}

string Server::getDisplayAddr()
{
	return displayAddr;
}

int Server::getStreamPort()
{
	return srvAddr.sin_port;
}

//��ȡ��ǩ������ݣ����Ӧ�ı�����
void serverList::getLabelMsg(string name, string buf)
{
	int index = 0;

	//����Ҫ���ݳ���������޸��ˣ��ƺ��޷����޸ķ�հ�
	//���ԡ���Ҫ�����ƺõ�Э�飡
	if (name == "type")index = 1;
	if (name == "protocol")index = 2;
	if (name == "hostname")index = 3;
	if (name == "port")index = 4;
	if (name == "display")index = 5;

	//��ȡ��ǩ�е�����
	srvArgs[index] = buf.substr(buf.find('>') + 1, buf.rfind('<') - buf.find('>') - 1);
}

//���ض�Ӧֵ
string serverList::getCfgByIndex(int index)
{
	return srvArgs[index];
}

//���캯��
Server::Server(string file)
{
	//VAR init
	fileName = file;
	mySrvList = new serverList;
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Initialize as a local Real-time Streaming server (Default Settings)
	srvAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &srvAddr.sin_addr);
	srvAddr.sin_port = htons(80);

	//���������ļ�
	readFile();
};

//��������
Server::~Server()
{
	closesocket(srvSocket);
}

//��ȡ�����ļ��������Ƿ�ɹ���ȡ
bool Server::readFile()
{
	fstream srvSettings;
	srvSettings.open(fileName, ios_base::in);  //ֻ���Ϳ���

	string buf;    //���뻺��
	string label;  //��ǩ�е�����

	if (!srvSettings.is_open())
	{
		//�ļ�δ�򿪻�δ�ҵ�������
		cout << "�Ҳ��������ļ��� " << srvSettingFile << " !" << endl;
		return false;
	}
	else
	{
		//�����ļ���β���Ժ�ɿ��Ǵ��ض�λ�ö���/������ǰ��ȡ״̬/ֱ�Ӵ浽�ڴ�֮��Ĵ�����
		while (!srvSettings.eof())
		{
			//����һ��Ҫ��һ���еģ������пո�֮��ӵ��Ķ���
			getline(srvSettings, buf);
			
			//1����������������Ϣ
			if (buf.find("<serverList>") != string::npos)
			{
				//ֻҪ�Ƿ�������ȫ��������������һ������������
				serverList *newServer = mySrvList, *nextServer = NULL;

				while (buf.find("</serverList>") == string::npos)
				{
					getline(srvSettings, buf);
					if (buf.find("<server>") != string::npos)
					{
						//��ȡ��������������Ϣ
						nextServer = new serverList;
						while (buf.find("</server>") == string::npos)
						{
							getline(srvSettings, buf);
							label = buf.substr(buf.find('<') + 1, buf.find('>') - buf.find('<') - 1);  //ȡ����ǩ�е�����

							nextServer->getLabelMsg(label, buf);               //���ݱ�ǩ�е��������
						}

						//����µķ������ڵ㣬����˫������
						newServer->next = nextServer;
						nextServer->prev = newServer;
						nextServer = nextServer->next;
						newServer = newServer->next;
					}
				}
			}

			//2����������������ӣ�
		}
	}

	return true;
};

//���ӷ��������Զ���ͷ��ʼѡ���һ�����õģ��������ϣ��ķ�������������
//������չ���л�����������
int Server::connectServer()
{
	serverList *waitToCnct = mySrvList->next;

	int isConnected = -1;
	//�ӷ����������ж��������Ϣ����������������
	while (waitToCnct != NULL)
	{
		//������ʹ����ʱ����������ȥ���ӣ�Connect�������ƱȽϸ��ӣ�����ΪȨ�������޷����ӵ�

		//ƴ������������·��
		string serverTmp;
		serverTmp = waitToCnct->getCfgByIndex(protocol) + "://" + waitToCnct->getCfgByIndex(hostName) + "/" + waitToCnct->getCfgByIndex(displayRoute);
		if (serverTmp.length() > 5)cout << "�������ӷ�������75�볬ʱ����" << serverTmp << endl;

		//����������������Ϸ���ô����أ�
		addrinfo *res;
		int errorCode = getaddrinfo(waitToCnct->getCfgByIndex(hostName).c_str(), NULL, NULL, &res);

		if (errorCode != 0)
		{
			cout << "��������ʧ�ܣ��Ƿ�������������룺" << WSAGetLastError() << endl;
		}
		else
		{
			//��д�ṹ��Ϣ���˿��Լ�д
			//TMD�� sockaddr��sockaddr_in��һ���£�ֻ����ǰ�߸�ͨ�ã�ֱ�ӰѼ������Դճ��ַ����ˣ�
			sockaddr_in srvAddrTmp = *(sockaddr_in *)res->ai_addr;
			srvAddrTmp.sin_port = htons(atoi(waitToCnct->getCfgByIndex(port).c_str()));

			inet_ntop(srvAddrTmp.sin_family, &srvAddrTmp.sin_addr, (char *)waitToCnct->srvArgs[hostAddr].data(), 17); //����������

			//�����ǿ��ܳ�ʱ�ģ�Ĭ�ϳ�ʱʱ����75��
			//To do list����һ��������ˣ���ɲ��а����������˵Ⱥܾúܾð�
			SOCKET socketTmp = socket(AF_INET, SOCK_STREAM, 0);
			isConnected = connect(socketTmp, (sockaddr *)&srvAddrTmp, sizeof(srvAddrTmp)); 	

			if (isConnected == 0)
			{
				cout << "--���ӳɹ�" << endl;
				srvAddr = srvAddrTmp;
				displayAddr = serverTmp;
				srvSocket = socketTmp;

				break;
			}
			else
			{
				cout << "--����ʧ�ܣ�������룺" << WSAGetLastError() << endl;
				waitToCnct = waitToCnct->next;  //�л���һ�������
			}
		}
	}

	if (isConnected != 0)
	{
		//���һ�鶼���ɹ���������޿��÷�����
		cout << "�޿��÷�����." << endl;
	}

	return isConnected;
}