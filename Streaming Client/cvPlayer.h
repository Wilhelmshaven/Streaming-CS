/*--Author������--*/

/*
	����OpenCV�Ĳ�������ģ�飩

	ʹ��OpenCV��ԭ�򣺱Ƚ���Ϥ����д��ݣ�Ҳ�ܹ����׻�ȡ�������룬�Ҹò��ֱ������ں�������

	��ƣ�
	һ��ת��ģ�飬��vector<int>�ṹ����������תΪ����
	һ��ͼ����У��洢������ͼ��
	һ�����������Ӷ�����ȡ��ͼ�񲢲��ţ�ͬʱ��ȡ������Ϣ
*/

#pragma once
#include "CommonHeaders.h"

#include "opencv2\highgui\highgui.hpp"

using namespace cv;

/*
	����OpenCV�Ĳ�����������ģʽ

	ʹ�ã�

*/
class cvPlayer
{
public:
	static cvPlayer* getInstance();

	int buildPlayer();

	int playBuffer(vector<int> *img);

	



	~cvPlayer();

private:

	int vecToMat(vector<int> &img);


	//OpenCV��Mat��ʽͼ����� 
	queue<Mat> imgQueue;

	/*
		����ģʽ���
	*/
	static cvPlayer *instance;
	cvPlayer();

	cvPlayer(const cvPlayer &);
	cvPlayer &operator=(const cvPlayer &);

	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo Garbo;
};