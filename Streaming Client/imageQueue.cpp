/*--Author：李宏杰--*/

#include "imageQueue.h"

imgBuffer *imgBuffer::instance = new imgBuffer;

imgBuffer *imgBuffer::getInstance()
{
	return instance;
}

/*
	完成类型转换并入队
	使用图像头的数据来将一维数组恢复回Mat矩阵格式
	代码经过辛苦的OpenCV测试得到
	首先使用数组初始化Mat并同时reshape，此时数据已恢复成数组，但存储模式不对
	所以第二步要转换矩阵模式
*/
void imgBuffer::pushBuffer(imgHead head, vector<int> img)
{
	Mat matrix = Mat(img).reshape(head.channels, head.cols);

	matrix.convertTo(matrix, head.imgType);

	imgQueue.push(matrix);
}

bool imgBuffer::popBuffer(Mat &img)
{
	img = imgQueue.front();

	imgQueue.pop();
}

bool imgBuffer::isBufEmpty()
{
	return imgQueue.empty();
}
