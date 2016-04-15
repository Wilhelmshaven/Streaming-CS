/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	服务端控制信令处理器，单例模式

	输入待解码的信令，然后解码
	最后把指令交给对应中间件传递给渲染器

	使用：
	
	void decodeMsg(string msg)：解码信令

	//void encodeMsg(imgHead head, unsigned int imgSize, unsigned int session)：编码信令

	bool getDecodedMsg(unsigned int &session, unsigned char &ctrlKey)：取出解码结果

	//bool getEncodedMsg(string &encodedMsg)：取出编码结果

*/
class ctrlMsgHandler
{
public:

	static ctrlMsgHandler* getInstance();

	void decodeMsg(SOCKET index, string msg);

	//void encodeMsg(imgHead head, unsigned int imgSize, unsigned int session);

	bool getDecodedMsg(SOCKET &index, unsigned int &session, unsigned char &ctrlKey);

	//bool getEncodedMsg(string &encodedMsg);

private:

	typedef struct decodedMsg
	{
		SOCKET index;

		unsigned int session;

		unsigned char ctrlKey;
	};

	queue<decodedMsg> decodedMsgQueue;

	//queue<string> encodedMsgQueue;
	
	//编码公共头
	string encodePublicHead(unsigned int payloadType, unsigned int session, unsigned int size);

	/*
		单例模式
	*/

	ctrlMsgHandler();

	static ctrlMsgHandler *instance;
	
	ctrlMsgHandler(const ctrlMsgHandler&);
	ctrlMsgHandler &operator=(const ctrlMsgHandler&);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo garbo;
};