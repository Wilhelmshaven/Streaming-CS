/*--Author������--*/

#include "cnctHandler.h"

cnctHandler *cnctHandler::instance = new cnctHandler(srvSettingFile);

/*
	���캯���������ļ���Ϊ�����������ȡ����ͷ��ָ�����ļ�·����
*/
cnctHandler::cnctHandler(string file)
{
	fileName = file;

	defaultSettings();
}

cnctHandler::cnctHandler()
{
	fileName = srvSettingFile;

	defaultSettings();
}

/*
	Ĭ�����ã�
	1������Winsock
	2���ѷ�������Ϣ��Ϊ���أ�LOCALHOST��
	3����ȡ�����������ļ�
*/
void cnctHandler::defaultSettings()
{
	mySrvList = new serverList;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);

	srvAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &srvAddr.sin_addr);
	srvAddr.sin_port = htons(80);

	//���������ļ�
	if (readFile())
	{
		cout << "��ȡ�����ļ��ɹ���" << endl;
	}
	else
	{
		cout << "��ȡ�����ļ�ʧ�ܣ�" << endl;
	}
}

/*
	���ӷ��������Զ���ͷ��ʼѡ���һ�����õģ��������ϣ��ķ�������������

	Todo�������л�����������
*/
int cnctHandler::connectServer()
{
	serverList *waitToCnct = mySrvList->next;

	//����ֵ�����ɹ����Ϊ0������Ϊ-1�������޿��÷�������������ֵ���������ϵ�������ʱ����connect�������صĴ����룬�μ�MSDN��
	int isConnected = -1;

	//�ӷ����������ж��������Ϣ����������������
	while (waitToCnct != NULL)
	{
		//Tip. ������ʹ����ʱ����������ȥ���ӣ�Connect�������ƱȽϸ��ӣ�����ΪȨ�������޷����ӵ�

		//ƴ������������·���ַ���
		string serverTmp;
		serverTmp = getCfgByIndex(protocol) + "://" + getCfgByIndex(hostName) + "/" + getCfgByIndex(displayRoute);
		if (serverTmp.length() > 5)cout << "�������ӷ�������75�볬ʱ����" << serverTmp << endl;

		/*
			��������
			Todo��������Ϸ���ô����أ�
		*/
		addrinfo *res;
		int errorCode = getaddrinfo(getCfgByIndex(hostName).c_str(), NULL, NULL, &res);

		if (errorCode != 0)
		{
			cout << "��������ʧ�ܣ��Ƿ�������������룺" << WSAGetLastError() << endl;
		}
		else
		{
			/*
				��д�ṹ��Ϣ���˿��Լ�д����ȫ�ֹ���ͷ��
				Tips. sockaddr��sockaddr_in��һ���£�ֻ����ǰ�߸�ͨ�ã�ֱ�ӰѼ������Դճ��ַ����ˣ�
			*/
			sockaddr_in srvAddrTmp = *(sockaddr_in *)res->ai_addr;
			srvAddrTmp.sin_port = htons(stoi(getCfgByIndex(port), nullptr, 10));

			//�����������ѽ���������IP���뵽�������б��и÷�������Ϣ����Ӧλ��
			inet_ntop(srvAddrTmp.sin_family, &srvAddrTmp.sin_addr, (char *)waitToCnct->srvArgs[hostAddr].data(), 17); 

			/*
				���ӷ������������ǿ��ܳ�ʱ�ģ�Ĭ�ϳ�ʱʱ����75��
				Todo����һ��������ˣ���ɲ��а����������˵Ⱥܾúܾð�
			*/
			SOCKET socketTmp = socket(AF_INET, SOCK_STREAM, 0);
			isConnected = connect(socketTmp, (sockaddr *)&srvAddrTmp, sizeof(srvAddrTmp));

			//�ڿ���̨�����Ӧ��Ϣ
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
				cout << "--����ʧ�ܣ�Winsock������룺" << WSAGetLastError() << endl;

				//�л���һ���������ֱ���ޱ�ѡ��������ָ��Ϊ�գ�
				waitToCnct = waitToCnct->next;  
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

cnctHandler* cnctHandler::getInstance()
{
	return instance;
}

SOCKET cnctHandler::getSocket()
{
	return srvSocket;
}

string cnctHandler::getDisplayAddr()
{
	return displayAddr;
}

int cnctHandler::getStreamPort()
{
	return srvAddr.sin_port;
}

serverList* cnctHandler::getSrvStruct()
{
	return mySrvList;
}

void cnctHandler::showSrvInfo()
{
	//�ڿ���̨��ʾ��ǰʹ�õķ�������Ϣ
	cout << "----��ǰʹ�õķ�������Ϣ----" << endl;
	cout << "���������ͣ�" << getCfgByIndex(srvType) << endl;
	cout << "���ŵ�ַ��" << getCfgByIndex(displayRoute) << endl;
	cout << "IP��ַ��" << getCfgByIndex(port) << endl;
	cout << "�˿ڣ�" << getCfgByIndex(srvType) << endl;
	cout << endl;
}

cnctHandler::~cnctHandler()
{
	closesocket(srvSocket);
}

/*
	��ȡ�����ļ�������ֵ�����Ƿ�ɹ���ȡ
*/
bool cnctHandler::readFile()
{
	//��ֻ���ļ�����ʽ���ļ�
	fstream srvSettings;
	srvSettings.open(fileName, ios_base::in); 

	if (!srvSettings.is_open())
	{
		//�ļ�δ�򿪻�δ�ҵ�������
		cout << "�Ҳ��������ļ��� " << srvSettingFile << " !" << endl;
		return false;
	}
	else
	{
		string buf;    //���뻺��
		string label;  //��ǩ�е�����

		//�����ļ���β
		//Todo���Ժ�ɿ��Ǵ��ض�λ�ö���/������ǰ��ȡ״̬/ֱ�Ӵ浽�ڴ�֮��Ĵ�����
		while (!srvSettings.eof())
		{
			//Tips. ����һ��Ҫ��һ���еģ������пո�֮��ӵ��Ķ���
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
							//ȡ����ǩ�е�����
							getline(srvSettings, buf);
							label = buf.substr(buf.find('<') + 1, buf.find('>') - buf.find('<') - 1);  

							//���ݱ�ǩ�е��������ṹ��
							getLabelMsg(label, buf);
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
			//Todo��...
		}
	}

	return true;
};

/*
	��ȡXML��ǩ������ݣ����Ӧ�ı�����

	�ṹ����һ���ַ��������������Ǹ��ݱ�ǩָ�������±�

	����Ҫ���ݳ���������޸��ˣ��ƺ��޷����޸ķ�հ�
	���ԡ���Ҫ�����ƺõ�Э�飡
*/
void cnctHandler::getLabelMsg(string name, string buf)
{
	int index = 0;

	if (name == "type")index = 1;
	if (name == "protocol")index = 2;
	if (name == "hostname")index = 3;
	if (name == "port")index = 4;
	if (name == "display")index = 5;

	//��ȡ��ǩ�е�����
	mySrvList->srvArgs[index] = buf.substr(buf.find('>') + 1, buf.rfind('<') - buf.find('>') - 1);
}

//���ط�������Ϣ�ṹ���ж�Ӧ�±������ֵ
string cnctHandler::getCfgByIndex(int index)
{
	return mySrvList->srvArgs[index];
}