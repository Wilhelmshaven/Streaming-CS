/*--Author������--*/

#include "rtpHandler.h"

//RTPģ�飺���RTP���ݰ��Ѿ�������
HANDLE hsRTPOutput = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);

rtpHandler *rtpHandler::instance = new rtpHandler;

rtpHandler * rtpHandler::getInstance()
{
	return instance;
}

rtpHandler::rtpHandler()
{

}

void rtpHandler::unpackRTP(string mediaPacket)
{
	/*
		ý������ȣ�8+12+4+12+MEIDA
		����ô��ͷ������
		Ҫ���ɴ಻ҪRTP����
		�������Ǽ����RTSP
	*/

	myImage image;

	/*
		������ȡ��ͼ��ͷ
		ֱ��ȡ����9-20��12�ֽڳ��ȣ�������
	*/
	auto head = (imgHead *)mediaPacket.substr(8, 12).c_str();

	image.head = *head;

	/*
		Ȼ����֤ʲô�ģ����ˣ��Ͳ�����
		���ԡ���RTP��Ϣֱͨͨ�Ӳ�Ҫ

		//TODO�������Ƿ�������
	*/

	/*
		�����ȡRTP��
	*/
	auto imgData = (vector<char> *)mediaPacket.substr(36, mediaPacket.length() - 36).c_str();

	image.img = *imgData;

	imageQueue.push(image);

	ReleaseSemaphore(hsRTPOutput, 1, NULL);
}

bool rtpHandler::getMedia(imgHead &head, vector<char>& img)
{
	if (imageQueue.empty())
	{
		return false;
	}

	myImage image = imageQueue.front();

	head = image.head;

	img = image.img;

	return true;
}