/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	SETUP֮��RTP���ݽ�ͨ����������RTSP�����TCP Socket���з��͡�RTP���ݽ������¸�ʽ���з�װ��

	| magic number | channel number | embedded data length | data |

	magic number - 1 byte value of hex 0x24
	RTP���ݱ�ʶ����"$"
	channel number - 1 byte value to denote the channel
	�ŵ����� - 1���ֽڣ�����ָʾ�ŵ�
	embedded data length - 2 bytes to denote the embedded data length
	���ݳ��� - 2���ֽڣ�����ָʾ�������ݳ���
	data - data packet, ie RTP packet, with the total length of the embedded data length
	���� - ���ݰ�������˵RTP�����ܳ�������������ݳ�����ͬ
	Below is a full example of the communication exchanged

	����ٸ���ͨ���ĵ�RTP��������һ����ͷ��
*/

/*
	RTP���ݰ������࣬������������뻺��

	ps.���ڽ��յ������ͨ���������ǣ�����һ���׽��֣�ý������һ���׽���
	����������Ӧ��UDP�����
	����ʹ��TCP�Ļ�����ô����RTP over RTSP�Ĵ��䷽ʽ
	��ʹ��ͬһ���׽��ֽ��д���
	���ԣ�����ͳһ������ģ�鸺������⵽��RTP���ݰ�������ת����ģ�鴦��

	ʹ�ã�

	void unpackRTP(string mediaPacket)������������ý�����ݰ�

	void getMedia(imgHead &head, vector<int> &img)������ý������

*/
class rtpHandler
{
public:

	static rtpHandler* getInstance();

	void unpackRTP(string mediaPacket);

	bool getMedia(imgHead &head, vector<char> &img);

private:

	typedef struct myImage
	{
		imgHead head;
		vector<char> img;
	};

	queue<myImage> imageQueue;

	/*
		����ģʽ���
	*/
	rtpHandler();

	static rtpHandler* instance;

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