#include "CommonHeaders.h"
#include "Server.h"
#include "myClient.h"

const rtspServer *rtspServer::instance = new rtspServer(servSettingFileName);

rtspServer::rtspServer(string file)
{
	this->fileName = file;

	// Initialize as a local server
	srvAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void *)&srvAddr.sin_addr);
	srvAddr.sin_port = 8554;

	readFile();
}

void rtspServer::readFile()
{
	fstream servSettings;
	servSettings.open(fileName, ios_base::in);

	string buf;

	if (!servSettings.is_open())
	{
		cout << "Can't find " << servSettingFileName << " !" << endl;
	}
	else
	{	
		while (!servSettings.eof())
		{
			servSettings >> buf;
			if (buf == "<protocol>RTSP</protocol>")
			{
				servSettings >> buf;
				buf = buf.substr(4, buf.length() - 9);
				cout << "Server: http://" << buf;
				inet_pton(srvAddr.sin_family, buf.c_str(), (void *)&srvAddr.sin_addr);

				servSettings >> buf;
				buf = buf.substr(6, buf.length() - 13);
				srvAddr.sin_port = atoi(buf.c_str());
				cout << ":" << buf << endl;	

				break;
			}
		}
	}
}