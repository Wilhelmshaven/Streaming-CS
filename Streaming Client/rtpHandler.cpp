/*--Author：李宏杰--*/

#include "rtpHandler.h"

//RTP模块：标记RTP数据包已经解包完成
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
	解包RTP包，就是去掉头部，取出图像数据即可

	基于TCP流式传输特点，一图一包
*/
void rtpHandler::unpackRTP(shared_ptr<vector<BYTE>> mediaPacket)
{
	/*
		媒体包长度：8+12+8+MEDIA

		RTPTCP+RTP+IMGHEAD
	*/

	myImage image;

	/*
		验证什么的，算了，就不做了
		所以……RTP信息通通直接不要

		TODO：让他们发挥作用

		首先提取出图像头
		直接取出第21-28共8字节长度，就是了
	*/
	auto head = (imgHead *)(&(*mediaPacket)[20]);

	image.head.channels = ntohs(head->channels);
	image.head.imgType = ntohs(head->imgType);
	image.head.xAxis.cols = ntohs(head->xAxis.cols);
	image.head.yAxis.rows = ntohs(head->yAxis.rows);

	/*
		最后提取RTP包
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
