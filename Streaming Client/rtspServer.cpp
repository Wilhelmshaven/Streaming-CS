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

//读取配置文件，配置文件应该是由服务器下发，保证其正确性。嗯暂时就不做太多的错误处理
void rtspServer::readFile()
{
	fstream servSettings;
	servSettings.open(fileName, ios_base::in);  //只读就可以

	string buf;  //输入缓存

	if (!servSettings.is_open())
	{
		//文件未打开或未找到，报错
		cout << "找不到配置文件： " << servSettingFileName << " !" << endl;
	}
	else
	{	
		//读到文件结尾，以后可考虑从特定位置读起/保留当前读取状态/直接存到内存之类的处理方法
		while (!servSettings.eof())
		{
			servSettings >> buf;

			//寻找RTSP服务器，之前其实还可以加一些判断，但是目前没必要
			if (buf == "<type>RTSP</type>")
			{
				string protocol, hostName, rtspPort, displayRoute; //地址头，域名，端口，播放地址
				int labelBegin, length;

				//暂时就这么啰嗦的分四段写着先吧，回头再优化
				//如果加错误处理，逐个检查，错误就抛出好了。域名靠解析，端口靠范围，其它的……

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

				// 将读取到的信息保存到服务器信息里
				//首先要用Winsock解析域名，回头补上这一行
				inet_pton(srvAddr.sin_family, hostName.c_str(), (void *)&srvAddr.sin_addr);   //保存IP
				srvAddr.sin_port = atoi(buf.c_str());                  //保存端口

				displayAddr = protocol + "://" + hostName + "/" + displayRoute;
				cout << "连接服务器：" << displayAddr << endl;

				//报告读取完成，连接去
				//................
				if(true)break;//如果成功连接上，就不用读了，否则继续找下一组服务器。这里得交互呀…………
			}
		}

		
		if (true)
		{
			//如果一组都不成功，就输出无可用服务器
			cout << "无可用服务器." << endl;
		}
	}
}