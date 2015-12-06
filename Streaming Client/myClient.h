#include "CommonHeaders.h"

class myMessage
{
public:
	myMessage();
};

//线程参数表
class rtspParam
{
public:
	string URI;
	int Port;
	int RTPPort;
	SOCKET socket;

	rtspParam()
	{
		Port = 554;
		socket = -1;
		RTPPort = RTP_PORT;
	}

	~rtspParam(){};
};