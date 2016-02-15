#pragma once
#include "CommonHeaders.h"

typedef struct rayImage
{
	string index;
	vector<int> image;
};

//ͼ�񻺴���
//����OpenCV��MAT��������ƣ�һ��ͷ�����֣�һ�����ݲ��֣�ֱ�Ӵ洢��һ�оͿ�����
//����ͼ������Ϊϡ�������
class imgBuffer
{
public:
	imgBuffer();

	//��ʼ�������С�����ߣ�ͨ����
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

	string format;           //ͼƬ��ʽ

	rayImage image;
};

//ͼ�������
//�����ά��һ��ͼ����У���Ⱦ����Ⱦ�õ�ͼ����ӣ�����ģ��ȡ����ͼ��
class imageQueue
{
public:
	imageQueue(int maxQueue);

	int pushQueue(imgBuffer img);    //ͼ�����
	imgBuffer popQueue();            //ͼ�����

	~imageQueue();

private:
	int maxLength;                  //�����г���

	HANDLE semaPhore;               //�ź���

	queue<imgBuffer> imgQueue;       //ͼ�����
};