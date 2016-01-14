#include "imageQueue.h"

/****
imgBuffer：自定义图像缓存结构
****/

//初始化图像缓存大小
imgBuffer::imgBuffer(int width, int height, int channel)
{
	this->width = width;
	this->height = height;
	this->channel = channel;

	//校验数据合理性
	if (this->width < 1)this->width = 1;
	if (this->height < 1)this->height = 1;
	if (this->channel < 1)this->channel = 1;
	if (this->channel > 4)this->channel = 4;

	//初始化缓存大小
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

//写入图像数据到缓存
void imgBuffer::writeImage(rayImage image)
{
	int width = image.img.size();
	int height = image.img[0].size();
	int channel = image.img[0][0].size();

	/**
	***校验数据合理性：若图像比缓存大，那么只保存局部
	***这个属于人为使用失误，我觉得不需要过多的校正或者说报错
	***应该由使用者来保证，你需要多大的图像就创建多大的缓存
	***如果需要多分辨率，那么你就应该创建多个不同大小的缓存
	**/
	if (width > this->width)width = this->width;
	if (height > this->height)height = this->height;
	if (channel > this->channel)channel = this->channel;

	//拷贝数据
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

//读取缓存
rayImage imgBuffer::readImage()
{
	return this->image;
}

/****
//imageQueue：自定义图像队列
//渲染好的图像入队，网络部分取图像出队
****/

//初始化队列长度，并创建信号量
imageQueue::imageQueue(int maxQueue)
{
	maxLength = maxQueue;

	semaPhore = CreateSemaphore(NULL, 0, maxLength, NULL);
}

//向队列中推入图像
int imageQueue::pushQueue(rayImage img)
{
	imgQueue.push(img);

	ReleaseSemaphore(semaPhore, 1, NULL);  //信号量增加1

	return 0;
}

//返回队尾的图像
rayImage imageQueue::popQueue()
{
	WaitForSingleObject(semaPhore, INFINITE);   //如果wait到了，就会给信号量减一（待验证）

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
