/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	RTP类，就是个打包功能
	输入待打包媒体数据，输出打包结果

	Usage：

	bool pack(SOCKET socket, imgHead head, vector<unsigned char> img)：传入套接字与图像数据编码RTP包，如果连接没有建立或者已经被Teardown，那么直接返回否

	bool getPacket(string &msg)：取出RTP包

*/
class rtpHandler
{
public:

	static rtpHandler* getInstance();

	//传入套接字与图像数据编码RTP包，如果连接没有建立或者已经被Teardown，那么直接返回否
	bool pack(SOCKET socket, imgHead head, vector<unsigned char> img);

	//取出RTP包
	bool getPacket(string &msg);

private:

	//为了非阻塞
	queue<string> packetQueue;

	/*
		rtp包中的包序列号管理器
	*/
	map<SOCKET, unsigned short> seqManager;

	unsigned short getSeqNumber(SOCKET socket);

	//随机序列号生成器，使用标准库实现
	unsigned short randSeq(SOCKET socket);

	/*
		编码各个头部
	*/
	void encodeRTPTCPHead(string &msg, size_t size);

	void encodeRTPHead(string &msg, SOCKET socket);

	void encodeImgHead(string &msg, imgHead head);

	map<SOCKET, unsigned long> timestampManager;

	unsigned long getTimeStamp(SOCKET socket);

	/*
		单例
	*/
	static rtpHandler* instance;

	rtpHandler();

	rtpHandler(const rtpHandler&);
	rtpHandler &operator=(const rtpHandler &);
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