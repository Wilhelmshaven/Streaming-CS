/*--Author������--*/

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
	�������ת�������
*/
void imgBuffer::pushBuffer(Mat img)
{
	myImg image;

	/*
		�Լ�����Mat��дͼ��ͷ��
	*/
	image.head.cols = img.cols;
	image.head.rows = img.rows;
	image.head.channels = img.channels();
	image.head.imgType = img.type();

	/*
		��OpenCV��Mat����תΪһά����
		�����ԣ���ʱ��ĩ�Ჹ��һ���ļ�϶��������Ʃ����4����8�ı�����Ҫ���ڴ���룩
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
