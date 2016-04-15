/*--Author������--*/

#include "cnctHandler.h"

//����ģ�飺�������Ϣ��Ҫ����
HANDLE hsNewSendMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctInput);

//����ģ�飺��ǽ��յ����µ�RTP��Ϣ
HANDLE hsNewRTPMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctRTPOutput);

//����ģ�飺��ǽ��յ����µ�RTSP��Ϣ
HANDLE hsNewRTSPMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctRTSPOutput);

cnctHandler *cnctHandler::instance = new cnctHandler(srvSettingFile);

queue<string> cnctHandler::sendMsgQueue, cnctHandler::recvRTPQueue, cnctHandler::recvRTSPQueue;

/*
	���캯���������ļ���Ϊ�����������ȡ����ͷ��ָ�����ļ�·����

	Ĭ�����ã�
	1������Winsock
	2���ѷ�������Ϣ��Ϊ���أ�loaclhost��
	3����ȡ�����������ļ�
*/
cnctHandler::cnctHandler(string fileName)
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);

	srvAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &srvAddr.sin_addr);
	srvAddr.sin_port = htons(80);

	//���������ļ�
	if (readFile(fileName))
	{
		cout << "��ȡ�����ļ��ɹ���" << endl;
	}
	else
	{
		cout << "��ȡ�����ļ�ʧ�ܣ�" << endl;
	}
}

cnctHandler* cnctHandler::getInstance()
{
	return instance;
}

/*
	��ȡ�����ļ�������ֵ�����Ƿ�ɹ���ȡ
*/
bool cnctHandler::readFile(string fileName)
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
	
	//���뻺��
	string buf;   

	//��ǩ�е�����
	string label;  

	//�����ļ���β
	//Todo���Ժ�ɿ��Ǵ��ض�λ�ö���/������ǰ��ȡ״̬/ֱ�Ӵ浽�ڴ�֮��Ĵ�����
	while (!srvSettings.eof())
	{
		//Tips. ����һ��Ҫ��һ���еģ������пո�֮��ӵ��Ķ���
		getline(srvSettings, buf);

		//1����������������Ϣ
		if (buf.find("<serverList>") != string::npos)
		{
			//ֻҪ�Ƿ�������ȫ��������

			while (buf.find("</serverList>") == string::npos)
			{
				getline(srvSettings, buf);

				if (buf.find("<server>") != string::npos)
				{
					//��ȡ��������������Ϣ
					srvInfo serverInfo;

					ZeroMemory(&serverInfo, sizeof(srvInfo));

					while (buf.find("</server>") == string::npos)
					{
						/*
							ȡ����ǩ�е�����
						*/

						getline(srvSettings, buf);
						label = buf.substr(buf.find('<') + 1, buf.find('>') - buf.find('<') - 1);

						/*
							���ݱ�ǩ�е��������ṹ��
						*/
						
						fillInfo(label, buf, serverInfo);
					}

					myServerList.push_back(serverInfo);
				}
			}
		}

		//2����������������ӣ�
		//Todo��...
	}
	
	return true;
}

/*
	����������Ϣ�ṹ��
*/
void cnctHandler::fillInfo(string label, string buf, srvInfo & serverInfo)
{
	string value = buf.substr(buf.find('>') + 1, buf.rfind('<') - buf.find('>') - 1);

	do
	{
		if (label == "srvName")
		{
			serverInfo.srvName = value;

			break;
		}

		if (label == "address")
		{
			serverInfo.address = value;

			break;
		}

		if (label == "protocol")
		{
			serverInfo.protocol = value;

			break;
		}

		if (label == "hostName")
		{
			serverInfo.hostName = value;

			/*
				��������/IP��ַ����¼
				Todo��������Ϸ���ô����أ�
			*/
			int errCode = getaddrinfo(value.c_str(), NULL, NULL, &(serverInfo.srv));

			if (errCode != 0)
			{
				serverInfo.isValid = false;
				//cout << "�����ļ��а����Ƿ�������������룺" << WSAGetLastError() << endl;
				//errHandler(201)
			}
			else
			{
				serverInfo.isValid = true;
			}

			break;
		}

		if (label == "port")
		{
			//ֱ������Winsock��ַ�ṹ	
			serverInfo.port = htons(stoul(value));

			break;
		}
	} while (0);
}

/*
	���ӷ��������Զ���ͷ��ʼѡ���һ�����õģ��������ϣ��ķ�������������

	Todo�������л�����������
*/
int cnctHandler::connectServer()
{
	//����ֵ�����ɹ����Ϊ0������Ϊ-1�������޿��÷�������������ֵ���������ϵ�������ʱ����connect�������صĴ����룬�μ�MSDN��
	int isConnected = -1;

	auto iter = myServerList.begin();
	auto end = myServerList.end();

	//�ӷ����������ж��������Ϣ����������������
	while (iter != end)
	{
		//��Ϣ���Ϸ�
		if (!iter->isValid)
		{
			++iter;
			continue;
		}

		cout << "�������ӷ�������75�볬ʱ����" << iter->address << endl;

		/*
			��д�ṹ��Ϣ��˿�
			Tips. sockaddr��sockaddr_in��һ���£�ֻ����ǰ�߸�ͨ�ã�ֱ�ӰѼ������Դճ��ַ����ˣ�
		*/

		sockaddr_in srvAddrTmp = *(sockaddr_in *)iter->srv->ai_addr;

		srvAddrTmp.sin_port = iter->port;

		/*
			���ӷ������������ǿ��ܳ�ʱ�ģ�Ĭ�ϳ�ʱʱ����75��
			Todo����һ��������ˣ���ɲ��а����������˵Ⱥܾúܾð�
		*/

		//Tips. ������ʹ����ʱ����������ȥ���ӣ�Connect�������ƱȽϸ��ӣ�����ΪȨ�������޷����ӵ�
		SOCKET socketTmp = socket(AF_INET, SOCK_STREAM, 0);

		isConnected = connect(socketTmp, (sockaddr *)&srvAddrTmp, sizeof(srvAddrTmp));

		//�ڿ���̨�����Ӧ��Ϣ
		if (isConnected == 0)
		{
			cout << "--���ӳɹ�" << endl;

			myServer = (*iter);

			srvSocket = socketTmp;

			//��ʾ��������Ϣ
			//showSrvInfo();

			//�����շ��߳�
			startThread();

			break;
		}
		else
		{
			cout << "--����ʧ�ܣ�Winsock������룺" << WSAGetLastError() << endl;

			//�л���һ���������ֱ���ޱ�ѡ��������ָ��Ϊ�գ�
			++iter;
		}
		
	}

	//���һ�鶼���ɹ���������޿��÷�����
	if (isConnected != 0)
	{		
		cout << "�޿��÷�����." << endl;
	}

	return isConnected;
}

string cnctHandler::getDisplayAddr()
{
	return myServer.address;
}

void cnctHandler::sendMessage(string msg)
{
	sendMsgQueue.push(msg);

	ReleaseSemaphore(hsNewSendMsg, 1, NULL);
}

bool cnctHandler::getRTPMessage(string & msg)
{
	if (recvRTPQueue.empty())
	{
		return false;
	}

	msg = recvRTPQueue.front();

	recvRTPQueue.pop();

	return true;
}

bool cnctHandler::getRTSPMessage(string & msg)
{
	if (recvRTSPQueue.empty())
	{
		return false;
	}

	msg = recvRTSPQueue.front();

	recvRTSPQueue.pop();

	return true;
}

cnctHandler::~cnctHandler()
{
	closesocket(srvSocket);
}

void cnctHandler::startThread()
{
	threadParam *param = new threadParam;

	param->socket = srvSocket;

	CreateThread(NULL, NULL, recvThread, param, NULL, NULL);

	CreateThread(NULL, NULL, sendThread, param, NULL, NULL);

	//����Socket���ճ�ʱΪ5�룬��������̫�ã��ŵ�Ȼ�������ڻ���������ģʽ���ͻ���ô��
	int recvTimeMax = 5000;
	setsockopt(srvSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)recvTimeMax, sizeof(int));
}

DWORD cnctHandler::sendThread(LPVOID lparam)
{
	threadParam *param = (threadParam *)lparam;

	SOCKET socket = param->socket;

	string msg;
	int bytesSent;

	while (1)
	{
		WaitForSingleObject(hsNewSendMsg, INFINITE);

		if (!sendMsgQueue.empty())
		{
			msg = sendMsgQueue.front();

			sendMsgQueue.pop();

			bytesSent = send(socket, msg.c_str(), msg.length(), NULL);

			if (bytesSent > 0)
			{
				cout << "Send Message:" << endl << msg << endl;
			}
		}
	}

	return 0;
}

/*
	�����̣߳��кܶ���Ҫ�޸ĵĵط�����Ҫ����ͼƬ
*/
DWORD cnctHandler::recvThread(LPVOID lparam)
{
	threadParam *param = (threadParam *)lparam;

	SOCKET socket = param->socket;

	string msg;
	
	string img;

	int bytesRecv;

	while (1)
	{
		msg.resize(BUF_SIZE);

		//TODO��������������⣬������
		bytesRecv = recv(socket, (char *)msg.data(), BUF_SIZE, NULL);
		
		if (bytesRecv >= 0)
		{
			msg = msg.substr(0, bytesRecv);
		}
		else
		{
			cout << WSAGetLastError() << endl;
			continue;
		}

		//����������յ�����Ϣ���ͣ�������Ӧ�Ķ��в������ź���
		
		if (msg.find("RTSP") != string::npos)
		{
			//RTSP����
			cout << "Recv:" << endl << msg << endl;

			recvRTSPQueue.push(msg);

			ReleaseSemaphore(hsNewRTSPMsg, 1, NULL);
		}

		//RTP����
		if (msg[0] == '$')
		{
			recvRTPQueue.push(img);

			ReleaseSemaphore(hsNewRTPMsg, 1, NULL);

			//New Img
			img.clear();
			img += msg;
				
		}
		else
		{
			img += msg;
		}
	}

	return 0;
}