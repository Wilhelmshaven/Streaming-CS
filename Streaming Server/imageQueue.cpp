#include "imageQueue.h"

/****
imgBuffer：自定义图像缓存结构
****/

imgBuffer::imgBuffer()
{
	width = 0;
	height = 0;
	channel = 0;
};

//初始化图像缓存大小
imgBuffer::imgBuffer(int width, int height, int channel, string format)
{
	this->width = width;
	this->height = height;
	this->channel = channel;
	this->format = format;

	//校验数据合理性
	//if (this->width < 1)this->width = 1;
	//if (this->height < 1)this->height = 1;
	//if (this->channel < 1)this->channel = 1;
	//if (this->channel > 4)this->channel = 4;
}

//拷贝构造函数
imgBuffer::imgBuffer(const imgBuffer &C)
{
	width = C.width;
	height = C.height;
	channel = C.channel;
	format = C.format;

	image = C.image;
}

//写入图像数据到缓存
void imgBuffer::writeImage(rayImage image)
{
	this->image = image;

	/**
	***校验数据合理性：若图像比缓存大，那么只保存局部
	***这个属于人为使用失误，我觉得不需要过多的校正或者说报错
	***应该由使用者来保证，你需要多大的图像就创建多大的缓存
	***如果需要多分辨率，那么你就应该创建多个不同大小的缓存
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
int imageQueue::pushQueue(imgBuffer img)
{
	imgQueue.push(img);

	ReleaseSemaphore(semaPhore, 1, NULL);  //信号量增加1

	return 0;
}

//返回队尾的图像
imgBuffer imageQueue::popQueue()
{
	WaitForSingleObject(semaPhore, INFINITE);   //如果wait到了，就会给信号量减一（待验证）

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
