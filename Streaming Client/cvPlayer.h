/*--Author：李宏杰--*/

/*
	基于OpenCV的播放器（模块）

	使用OpenCV的原因：比较熟悉，编写快捷，也能够轻易获取键盘输入，且该部分本身不属于核心内容

	设计：
	一个转换模块，把vector<int>结构传来的数据转为矩阵
	一个图像队列，存储到来的图像
	一个播放器，从队列中取出图像并播放，同时获取键盘消息
*/

#pragma once
#include "CommonHeaders.h"

#include "opencv2\highgui\highgui.hpp"

using namespace cv;

/*
	基于OpenCV的播放器，单例模式

	使用：

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


	//OpenCV的Mat格式图像队列 
	queue<Mat> imgQueue;

	/*
		单例模式相关
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