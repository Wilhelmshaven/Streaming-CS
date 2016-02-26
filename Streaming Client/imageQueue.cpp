/*--Author������--*/

#include "imageQueue.h"

imgBuffer *imgBuffer::instance = new imgBuffer;

imgBuffer *imgBuffer::getInstance()
{
	return instance;
}

/*
	�������ת�������
	ʹ��ͼ��ͷ����������һά����ָ���Mat�����ʽ
	���뾭�������OpenCV���Եõ�
	����ʹ�������ʼ��Mat��ͬʱreshape����ʱ�����ѻָ������飬���洢ģʽ����
	���Եڶ���Ҫת������ģʽ
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
