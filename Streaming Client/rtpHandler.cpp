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

/*
	���RTP��������ȥ��ͷ����ȡ��ͼ�����ݼ���

	����TCP��ʽ�����ص㣬һͼһ��
*/
void rtpHandler::unpackRTP(string mediaPacket)
{
	/*
		ý������ȣ�4+12+8+MEDIA

		RTPTCP+RTP+IMGHEAD
	*/

	myImage image;

	/*
		��֤ʲô�ģ����ˣ��Ͳ�����
		���ԡ���RTP��Ϣֱͨͨ�Ӳ�Ҫ
		
		TODO�������Ƿ�������

		������ȡ��ͼ��ͷ
		ֱ��ȡ����17-24��8�ֽڳ��ȣ�������	
	*/
	auto head = (imgHead *)(mediaPacket.c_str() + 16);

	image.head.channels = ntohs(head->channels);
	image.head.imgType = ntohs(head->imgType);
	image.head.xAxis.cols = ntohs(head->xAxis.cols);
	image.head.yAxis.rows = ntohs(head->yAxis.rows);

	/*
		�����ȡRTP��
	*/
	
	image.head = (*head);

	image.img.resize(mediaPacket.size() - 24);
	memcpy(&image.img[0], mediaPacket.c_str() + 24, mediaPacket.size() - 24);

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

	imageQueue.pop();

	return true;
}