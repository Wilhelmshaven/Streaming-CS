#include "CommonHeaders.h"
#include "rtspServer.h"
#include "myClient.h"

const rtspServer *rtspServer::instance = new rtspServer(servSettingFileName);

rtspServer::rtspServer(string file)
{
	this->fileName = file;

	// Initialize as a local server (Default Settings)
	srvAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void *)&srvAddr.sin_addr);
	srvAddr.sin_port = 8554;

	// Read setting file
	readFile();
}

//��ȡ�����ļ��������ļ�Ӧ�����ɷ������·�����֤����ȷ�ԡ�����ʱ�Ͳ���̫��Ĵ�����
void rtspServer::readFile()
{
	fstream servSettings;
	servSettings.open(fileName, ios_base::in);  //ֻ���Ϳ���

	string buf;  //���뻺��

	if (!servSettings.is_open())
	{
		//�ļ�δ�򿪻�δ�ҵ�������
		cout << "�Ҳ��������ļ��� " << servSettingFileName << " !" << endl;
	}
	else
	{	
		//�����ļ���β���Ժ�ɿ��Ǵ��ض�λ�ö���/������ǰ��ȡ״̬/ֱ�Ӵ浽�ڴ�֮��Ĵ�����
		while (!servSettings.eof())
		{
			servSettings >> buf;

			//Ѱ��RTSP��������֮ǰ��ʵ�����Լ�һЩ�жϣ�����Ŀǰû��Ҫ
			if (buf == "<type>RTSP</type>")
			{
				string protocol, hostName, rtspPort, displayRoute; //��ַͷ���������˿ڣ����ŵ�ַ
				int labelBegin, length;

				//��ʱ����ô���µķ��Ķ�д���Ȱɣ���ͷ���Ż�
				//����Ӵ����������飬������׳����ˡ��������������˿ڿ���Χ�������ġ���

				servSettings >> buf;				
				labelBegin = buf.find("protocol");
				if (labelBegin != string::npos)
				{
					protocol = buf.substr(labelBegin + 9, buf.length() - 21);
					//cout << protocol<<endl;
				}
				
				servSettings >> buf;
				labelBegin = buf.find("hostname");
				if (labelBegin != string::npos)
				{
					hostName = buf.substr(labelBegin + 9, buf.length() - 21);
					//cout << hostName<<endl;
				}
				
				servSettings >> buf;
				labelBegin = buf.find("port");
				if (labelBegin != string::npos)
				{
					rtspPort = buf.substr(labelBegin + 5, buf.length() - 13);
					//cout << rtspPort<<endl;
				}
				
				servSettings >> buf;
				labelBegin = buf.find("display");
				if (labelBegin != string::npos)
				{
					displayRoute = buf.substr(labelBegin + 8, buf.length() - 19);
					//cout << displayRoute<<endl;
				}

				// ����ȡ������Ϣ���浽��������Ϣ��
				//����Ҫ��Winsock������������ͷ������һ��
				inet_pton(srvAddr.sin_family, hostName.c_str(), (void *)&srvAddr.sin_addr);   //����IP
				srvAddr.sin_port = atoi(buf.c_str());                  //����˿�

				displayAddr = protocol + "://" + hostName + "/" + displayRoute;
				cout << "���ӷ�������" << displayAddr << endl;

				//�����ȡ��ɣ�����ȥ
				//................
				if(true)break;//����ɹ������ϣ��Ͳ��ö��ˣ������������һ�������������ý���ѽ��������
			}
		}

		
		if (true)
		{
			//���һ�鶼���ɹ���������޿��÷�����
			cout << "�޿��÷�����." << endl;
		}
	}
}