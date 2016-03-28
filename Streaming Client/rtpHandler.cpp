/*--Author：李宏杰--*/

#include "rtpHandler.h"

//RTP模块：标记RTP数据包已经解包完成
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
		媒体包长度：8+12+4+12+MEIDA
		真特么多头部……
		要不干脆不要RTP好了
		交互还是坚持用RTSP
	*/

	myImage image;

	/*
		首先提取出图像头
		直接取出第9-20共12字节长度，就是了
	*/
	auto head = (imgHead *)mediaPacket.substr(8, 12).c_str();

	image.head = *head;

	/*
		然后，验证什么的，算了，就不做了
		所以……RTP信息通通直接不要

		//TODO：让他们发挥作用
	*/

	/*
		最后提取RTP包
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