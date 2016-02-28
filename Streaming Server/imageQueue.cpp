/*--Author：李宏杰--*/

#include "imageQueue.h"

imgBuffer *imgBuffer::instance = new imgBuffer;

imgBuffer *imgBuffer::getInstance()
{
	return instance;
}

imgBuffer::imgBuffer()
{

}

/*
	完成类型转换并入队
*/
void imgBuffer::pushBuffer(Mat img)
{
	myImg image;

	/*
		自己根据Mat填写图像头部
	*/
	image.head.cols = img.cols;
	image.head.rows = img.rows;
	image.head.channels = img.channels();
	image.head.imgType = img.type();

	/*
		把OpenCV的Mat矩阵转为一维数组
		连续性：有时行末会补上一定的间隙，以满足譬如是4或者8的倍数的要求（内存对齐）
	*/
	image.img= img.reshape(1, 1);

	imgQueue.push(image);
}

bool imgBuffer::popBuffer(imgHead & head, vector<int> & img)
{
	if (isBufEmpty())
	{
		return false;
	}
	else
	{
		head = imgQueue.front().head;

		img = imgQueue.front().img;

		imgQueue.pop();

		return true;
	}
}


bool imgBuffer::isBufEmpty()
{
	return imgQueue.empty();
}
