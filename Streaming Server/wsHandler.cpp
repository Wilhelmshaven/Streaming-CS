/*--Author：李宏杰--*/

//Websocket使用
#include "sha1\sha1.h"

#include "wsHandler.h"

namespace wsNS
{
	HANDLE hsWebMsgArrived = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);

	HANDLE hsWebHandshake = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::webHandshake);
};

using namespace wsNS;

wsHandler *wsHandler::instance = new wsHandler;

queue<myWSMsg> wsHandler::msgInQueue, wsHandler::msgOutQueue;

wsHandler * wsHandler::getInstance()
{
	return instance;
}

void wsHandler::decodeMsg(SOCKET socket, string msg)
{
	myWSMsg tmp;

	tmp.index = socket;
	tmp.msg = msg;

	msgInQueue.push(tmp);

	ReleaseSemaphore(hsWebMsgArrived, 1, NULL);
}

bool wsHandler::getResponse(SOCKET & socket, string & msg)
{
	if (msgOutQueue.empty())
	{
		return false;
	}

	myWSMsg tmp = msgOutQueue.front();

	socket = tmp.index;

	msg = tmp.msg;

	msgOutQueue.pop();

	return true;
}


DWORD wsHandler::decodeThreadFunc(LPVOID lparam)
{
	SHA1 encrypt;

	myWSMsg message;

	const string responseMsg = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";

	string key;

	const string selfDefineKey = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	const string clientWsKeyTitle = "Sec-WebSocket-Key: ";
	const int length = clientWsKeyTitle.length();

	while (1)
	{
		WaitForSingleObject(hsWebMsgArrived, INFINITE);

		if (msgInQueue.empty())
		{
			continue;
		}

		message = msgInQueue.front();
		msgInQueue.pop();

		/*
			获取客户端的随机码
		*/
		string tmp = message.msg;
		tmp = tmp.substr(tmp.find(clientWsKeyTitle));

		tmp = tmp.substr(length, tmp.find("\r") - length);

		//拼接
		key = tmp + selfDefineKey;

		/*
			运算
		*/

		string buf;
		buf.resize(400);
		encrypt.SHA_GO(key.c_str(), (char *)buf.data());

		string srvKey = base64(buf.c_str(), 20);

		message.msg = responseMsg + srvKey + "\r\n\r\n";

		msgOutQueue.push(message);

		ReleaseSemaphore(hsWebHandshake, 1, NULL);
	}

	return 0;
}

wsHandler::wsHandler()
{
	CreateThread(NULL, NULL, decodeThreadFunc, NULL, NULL, NULL);
}

string wsHandler::base64(const char* Data, int DataByte)
{
	//编码表
	const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	string strEncode;
	unsigned char Tmp[4] = { 0 };
	int LineLength = 0;
	for (int i = 0; i<(int)(DataByte / 3); i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
		if (LineLength += 4, LineLength == 76) { strEncode += "\r\n"; LineLength = 0; }
	}
	//对剩余数据进行编码
	int Mod = DataByte % 3;
	if (Mod == 1)
	{
		Tmp[1] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += "=";
	}

	return strEncode;
}