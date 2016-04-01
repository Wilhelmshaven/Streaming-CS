/*--Author������--*/

#include "rtpHandler.h"

//RTPģ�飺���RTP���ݰ��Ѿ�������
HANDLE hsRTPOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::rtpOutput);

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

		Ȼ����֤ʲô�ģ����ˣ��Ͳ�����
		���ԡ���RTP��Ϣֱͨͨ�Ӳ�Ҫ

		//TODO�������Ƿ�������
	*/
	auto head = (imgHead *)mediaPacket.substr(8, 12).c_str();

	image.head = *head;

	/*
		�����ȡRTP��
	*/
	image.img.resize(mediaPacket.size() - 36);

	memcpy(&image.img[0], &mediaPacket[36], mediaPacket.size() - 36);

	imageQueue.push(image);

	ReleaseSemaphore(hsRTPOutput, 1, NULL);
}

bool rtpHandler::getMedia(imgHead &head, vector<unsigned char>& img)
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