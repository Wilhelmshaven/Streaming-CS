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
	��OpenCV��Mat����תΪһά����
	����Ҫ��ͼ��ͷ��ͼ�����ݹ�������
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
