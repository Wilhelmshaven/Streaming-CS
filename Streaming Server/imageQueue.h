#pragma once
#include "CommonHeaders.h"

typedef struct rayImage
{
	string index;
	vector<int> image;
};

//图像缓存类
//仿照OpenCV的MAT类重新设计，一个头部部分，一个数据部分，直接存储成一行就可以了
//差异图可以作为稀疏矩阵传输
class imgBuffer
{
public:
	imgBuffer();

	//初始化缓存大小：宽，高，通道数
	imgBuffer(int width, int height, int channel, string format); 

	imgBuffer(const imgBuffer &C);

	void writeImage(rayImage img);

	int getWidth();
	int getHeight();
	int getChannel();
	string getFormat();

	rayImage getImage();

private:
	int width;
	int height;
	int channel;

	string format;           //图片格式

	rayImage image;
};

//图像队列类
//管理和维护一个图像队列，渲染器渲染好的图像入队，网络模块取出队图像
class imageQueue
{
public:
	imageQueue(int maxQueue);

	int pushQueue(imgBuffer img);    //图像入队
	imgBuffer popQueue();            //图像出队

	~imageQueue();

private:
	int maxLength;                  //最大队列长度

	HANDLE semaPhore;               //信号量

	queue<imgBuffer> imgQueue;       //图像队列
};