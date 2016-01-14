#pragma once
#include "CommonHeaders.h"

//ͼ��ṹ
//ȷ����׼����ά���飬��*��*ͨ����
//imgVector: vector<vector<vector<int>>>
typedef struct rayImage
{
	imgVector img;
	string index;              //�ỰID��Session��
};

//ͼ�񻺴棨�ࣩ
class imgBuffer
{
public:
	//��ʼ�������С
	imgBuffer(int width, int height, int channel);   

	void writeImage(rayImage img);     //д��ͼ��
	rayImage readImage();              //��ȡͼ��

private:
	int width;
	int height;
	int channel;

	//string format;           //ͼƬ��ʽ��ѡ��)
	//string codec;            //���������ƣ�ѡ�

	rayImage image;
};

//ͼ�������
//�����ά��һ��ͼ����У���Ⱦ����Ⱦ�õ�ͼ����ӣ�����ģ��ȡ����ͼ��
class imageQueue
{
public:
	imageQueue(int maxQueue);

	int pushQueue(rayImage img);    //ͼ�����
	rayImage popQueue();            //ͼ�����

	~imageQueue();

private:
	int maxLength;                  //�����г���

	HANDLE semaPhore;               //�ź���

	queue<rayImage> imgQueue;       //ͼ�����
};