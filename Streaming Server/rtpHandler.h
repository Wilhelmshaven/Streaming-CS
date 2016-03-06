/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	RTP类，就是个打包功能
	输入待打包媒体数据，输出打包结果

	Usage：

	string pack(SOCKET socket, vector<int> img)：传入套接字与图像数据，返回编码好的RTP包

*/

class rtpHandler
{
public:

	static rtpHandler* getInstance();

	//传入套接字与图像数据，返回编码好的RTP包
	string pack(SOCKET socket, vector<int> img);

private:

	/*
		序列号管理器
	*/
	map<SOCKET, unsigned short> seqManager;

	short getSeqNumber(SOCKET socket);

	//随机序列号生成器，使用标准库实现
	unsigned short randSeq(SOCKET socket);

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