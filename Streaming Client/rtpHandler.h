/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

//����JRTPLIB
#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"

using namespace jrtplib;

/*
	ʹ��JRTPLIB�����RTP���ݰ��Ľ������������
	��Ҫ����ģ���ṩĿ��������ĵ�ַ��˿���Ϣ
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