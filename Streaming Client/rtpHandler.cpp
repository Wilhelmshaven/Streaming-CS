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
void rtpHandler::unpackRTP(shared_ptr<vector<BYTE>> mediaPacket)
{
	/*
		ý������ȣ�8+12+8+MEDIA

		RTPTCP+RTP+IMGHEAD
	*/

	myImage image;

	/*
		��֤ʲô�ģ����ˣ��Ͳ�����
		���ԡ���RTP��Ϣֱͨͨ�Ӳ�Ҫ

		TODO�������Ƿ�������

		������ȡ��ͼ��ͷ
		ֱ��ȡ����21-28��8�ֽڳ��ȣ�������
	*/
	auto head = (imgHead *)(&(*mediaPacket)[20]);

	image.head.channels = ntohs(head->channels);
	image.head.imgType = ntohs(head->imgType);
	image.head.xAxis.cols = ntohs(head->xAxis.cols);
	image.head.yAxis.rows = ntohs(head->yAxis.rows);

	/*
		�����ȡRTP��
	*/

	auto iter = (*mediaPacket).begin();

	(*mediaPacket).erase(iter, iter + 28);

	image.img = mediaPacket;

	imageQueue.push(image);

	ReleaseSemaphore(hsRTPOutput, 1, NULL);
}

bool rtpHandler::getMedia(imgHead & head, shared_ptr<vector<BYTE>>& img)
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
