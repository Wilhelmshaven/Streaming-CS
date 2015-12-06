#include "head.h"

#define BUF_SIZE 8196

int main()
{
	//初始化Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建Socket;
	SOCKET rtspSocket = socket(AF_INET, SOCK_STREAM, 0);

	in_addr address;
	sockaddr_in ServerAddr;

	ServerAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.1.101", &address);
	ServerAddr.sin_addr = address;
	ServerAddr.sin_port = htons(atoi("554"));

	int flag = connect(rtspSocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr));//这里是可能超时的，默认超时时间是75秒

	if (flag == 0)
	{
		char *msg = new char[BUF_SIZE];
		char *response = new char[BUF_SIZE];
		char *substr = new char[BUF_SIZE];
		ZeroMemory(substr, BUF_SIZE);
		char *track = new char[BUF_SIZE];
		ZeroMemory(track, BUF_SIZE);
		char *Session = new char[BUF_SIZE];
		ZeroMemory(Session, BUF_SIZE);
		string URI = "rtsp://192.168.1.101:554/test.mp3";
		int Seq = 0, errCode;

		++Seq;
		sprintf_s(msg, BUF_SIZE, "OPTIONS %s RTSP/1.0\r\nCSeq: %d\r\n\r\n", URI.c_str(), Seq);
		send(rtspSocket, msg, strlen(msg), 0);
		cout << "Send: " << msg << endl;

		ZeroMemory(response, BUF_SIZE);
		errCode = recv(rtspSocket, response, BUF_SIZE, 0);    //接收信令
		cout << "Recv: " << response << endl;

		++Seq;
		sprintf_s(msg, BUF_SIZE, "DESCRIBE %s RTSP/1.0\r\nCSeq: %d\r\nAccept: application/sdp\r\n\r\n", URI.c_str(), Seq);
		send(rtspSocket, msg, strlen(msg), 0);
		cout << "Send: " << msg << endl;

		ZeroMemory(response, BUF_SIZE);
		errCode = recv(rtspSocket, response, BUF_SIZE, 0);    //接收信令
		cout << "Recv: " << response << endl;

		substr = strstr(response, "m=audio");  //只有视频的话，m=video
		substr = strstr(substr, "a=control:");
		for (int i = 0;; i++)
		{
			if (substr[i + 10] != '\r')track[i] = substr[i + 10];
			else break;
		}

		++Seq;
		sprintf_s(msg, BUF_SIZE, "SETUP %s/%s RTSP/1.0\r\nCSeq: %d\r\nTransport: RTP/AVP;unicast;client_port=%d-%d\r\n\r\n",
			URI.c_str(), track, Seq, 55358, 55359);
		send(rtspSocket, msg, strlen(msg), 0);
		cout << "Send: " << msg << endl;

		ZeroMemory(response, BUF_SIZE);
		errCode = recv(rtspSocket, response, BUF_SIZE, 0);    //接收信令
		cout << "Recv: " << response << endl;
		char *SetupHead = "Session: ";
		char *tmp = strstr(response, SetupHead);
		if (tmp != NULL)
		{
			for (int i = 0;; i++)
			{
				if (tmp[i + 9] == ';')break;
				Session[i] = tmp[i + 9];
			}

			++Seq;
			sprintf_s(msg, BUF_SIZE, "PLAY %s RTSP/1.0\r\nCSeq: %d\r\nSession: %s\r\nRange: npt=0.000-\r\n\r\n", URI.c_str(), Seq, Session);
			send(rtspSocket, msg, strlen(msg), 0);
			cout << "Send: " << msg << endl;

			ZeroMemory(response, BUF_SIZE);
			errCode = recv(rtspSocket, response, BUF_SIZE, 0);    //接收信令
			cout << "Recv: " << response << endl;
		}
	}


	return 0;
}