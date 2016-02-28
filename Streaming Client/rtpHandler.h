/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

//加载JRTPLIB
#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"

using namespace jrtplib;

/*
	使用JRTPLIB库完成RTP数据包的接收与解析工作
	需要网络模块提供目标服务器的地址与端口信息
*/
class rtpHandler
{
public:

private:

	/*
		Singleton
	*/
	static rtpHandler* instance;

	rtpHandler();

	rtpHandler(const rtpHandler&);
	rtpHandler &operator = (const rtpHandler&);
	class CGarbo
	{
	public:
		CGarbo()
		{
			if (instance)delete instance;
		}
	};
	CGarbo garbo;
};