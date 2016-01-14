#include "imageQueue.h"

/****
imgBuffer���Զ���ͼ�񻺴�ṹ
****/

//��ʼ��ͼ�񻺴��С
imgBuffer::imgBuffer(int width, int height, int channel)
{
	this->width = width;
	this->height = height;
	this->channel = channel;

	//У�����ݺ�����
	if (this->width < 1)this->width = 1;
	if (this->height < 1)this->height = 1;
	if (this->channel < 1)this->channel = 1;
	if (this->channel > 4)this->channel = 4;

	//��ʼ�������С
	image.img.resize(width);
	for (int i = 0; i < width; ++i)
	{
		image.img[i].resize(height);
		for (int j = 0; j < height; ++j)
		{
			image.img[i][j].resize(channel);
		}
	}
}

//д��ͼ�����ݵ�����
void imgBuffer::writeImage(rayImage image)
{
	int width = image.img.size();
	int height = image.img[0].size();
	int channel = image.img[0][0].size();

	/**
	***У�����ݺ����ԣ���ͼ��Ȼ������ôֻ����ֲ�
	***���������Ϊʹ��ʧ���Ҿ��ò���Ҫ�����У������˵����
	***Ӧ����ʹ��������֤������Ҫ����ͼ��ʹ������Ļ���
	***�����Ҫ��ֱ��ʣ���ô���Ӧ�ô��������ͬ��С�Ļ���
	**/
	if (width > this->width)width = this->width;
	if (height > this->height)height = this->height;
	if (channel > this->channel)channel = this->channel;

	//��������
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			for (int k = 0; k < channel; ++k)
			{
				this->image.img[i][j][k] = image.img[i][j][k];
			}
		}
	}
}

//��ȡ����
rayImage imgBuffer::readImage()
{
	return this->image;
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
int imageQueue::pushQueue(rayImage img)
{
	imgQueue.push(img);

	ReleaseSemaphore(semaPhore, 1, NULL);  //�ź�������1

	return 0;
}

//���ض�β��ͼ��
rayImage imageQueue::popQueue()
{
	WaitForSingleObject(semaPhore, INFINITE);   //���wait���ˣ��ͻ���ź�����һ������֤��

	rayImage img;

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
