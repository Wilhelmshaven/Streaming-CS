#pragma once
#include "CommonHeaders.h"

//图像结构
//确定标准：三维数组，宽*长*通道数
//imgVector: vector<vector<vector<int>>>
typedef struct rayImage
{
	imgVector img;
	string index;              //会话ID（Session）
};

//图像缓存（类）
class imgBuffer
{
public:
	//初始化缓存大小
	imgBuffer(int width, int height, int channel);   

	void writeImage(rayImage img);     //写入图像
	rayImage readImage();              //读取图像

private:
	int width;
	int height;
	int channel;

	//string format;           //图片格式（选填)
	//string codec;            //编码器名称（选填）

	rayImage image;
};

//图像队列类
//管理和维护一个图像队列，渲染器渲染好的图像入队，网络模块取出队图像
class imageQueue
{
public:
	imageQueue(int maxQueue);

	int pushQueue(rayImage img);    //图像入队
	rayImage popQueue();            //图像出队

	~imageQueue();

private:
	int maxLength;                  //最大队列长度

	HANDLE semaPhore;               //信号量

	queue<rayImage> imgQueue;       //图像队列
};