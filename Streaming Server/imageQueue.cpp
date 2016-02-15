#include "imageQueue.h"

/****
imgBuffer���Զ���ͼ�񻺴�ṹ
****/

imgBuffer::imgBuffer()
{
	width = 0;
	height = 0;
	channel = 0;
};

//��ʼ��ͼ�񻺴��С
imgBuffer::imgBuffer(int width, int height, int channel, string format)
{
	this->width = width;
	this->height = height;
	this->channel = channel;
	this->format = format;

	//У�����ݺ�����
	//if (this->width < 1)this->width = 1;
	//if (this->height < 1)this->height = 1;
	//if (this->channel < 1)this->channel = 1;
	//if (this->channel > 4)this->channel = 4;
}

//�������캯��
imgBuffer::imgBuffer(const imgBuffer &C)
{
	width = C.width;
	height = C.height;
	channel = C.channel;
	format = C.format;

	image = C.image;
}

//д��ͼ�����ݵ�����
void imgBuffer::writeImage(rayImage image)
{
	this->image = image;

	/**
	***У�����ݺ����ԣ���ͼ��Ȼ������ôֻ����ֲ�
	***���������Ϊʹ��ʧ���Ҿ��ò���Ҫ�����У������˵����
	***Ӧ����ʹ��������֤������Ҫ����ͼ��ʹ������Ļ���
	***�����Ҫ��ֱ��ʣ���ô���Ӧ�ô��������ͬ��С�Ļ���
	**/
}

int imgBuffer::getWidth()
{
	return width;
}

int imgBuffer::getHeight()
{
	return height;
}

int imgBuffer::getChannel()
{
	return channel;
}

string imgBuffer::getFormat()
{
	return format;
}

rayImage imgBuffer::getImage()
{
	return image;
}

/****
//imageQueue���Զ���ͼ�����
//��Ⱦ�õ�ͼ����ӣ����粿��ȡͼ�����
****/

//��ʼ�����г��ȣ��������ź���
imageQueue::imageQueue(int maxQueue)
{
	maxLength = maxQueue;

	semaPhore = CreateSemaphore(NULL, 0, maxLength, NULL);
}

//�����������ͼ��
int imageQueue::pushQueue(imgBuffer img)
{
	imgQueue.push(img);

	ReleaseSemaphore(semaPhore, 1, NULL);  //�ź�������1

	return 0;
}

//���ض�β��ͼ��
imgBuffer imageQueue::popQueue()
{
	WaitForSingleObject(semaPhore, INFINITE);   //���wait���ˣ��ͻ���ź�����һ������֤��

	imgBuffer img;

	if (imgQueue.empty())
	{
		img = imgQueue.front();
		imgQueue.pop();
	}

	return img;
}

imageQueue::~imageQueue()
{
	CloseHandle(semaPhore);
}
