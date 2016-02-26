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
	把OpenCV的Mat矩阵转为一维数组
	但是要把图像头和图像数据关联起来
*/
void imgBuffer::pushBuffer(imgHead head, Mat img)
{
	myImg image;
	image.head = head;
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
