/*--Author������--*/

#include "imgBuffer.h"

//ͼ����У���ǻ�������ͼ���ˣ�������
HANDLE hsImageReady = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::imgBufferOutput);

imgBuffer *imgBuffer::instance = new imgBuffer;

imgBuffer *imgBuffer::getInstance()
{
	return instance;
}

imgBuffer::imgBuffer()
{

}

void imgBuffer::pushBuffer(SOCKET index, imgHead head, shared_ptr<vector<BYTE>> img)
{
	myImg image;

	image.head = head;
	image.img = img;

	image.index = index;

	imgQueue.push(image);

	ReleaseSemaphore(hsImageReady, 1, NULL);
}

bool imgBuffer::popBuffer(SOCKET &index, imgHead & head, shared_ptr<vector<BYTE>> & img)
{
	if (isBufEmpty())
	{
		return false;
	}

	myImg image = imgQueue.front();
	imgQueue.pop();

	head = image.head;
	img = image.img;

	index = image.index;

	return true;
}

bool imgBuffer::isBufEmpty()
{
	return imgQueue.empty();
}

int imgBuffer::dropFrame(unsigned int cnt)
{
	for (int i = 0; i < cnt; ++i)
	{
		if (isBufEmpty())
		{
			return i;
		}

		imgQueue.pop();
	}

	return 0;
}