/*--Author：李宏杰--*/

#include "imgBuffer.h"

//图像队列：标记缓存中有图像了，请拿走
HANDLE hsImageReady = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::imgBufferOutput);

imgBuffer *imgBuffer::instance = new imgBuffer;

imgBuffer *imgBuffer::getInstance()
{
	return instance;
}

imgBuffer::imgBuffer()
{

}

void imgBuffer::pushBuffer(imgHead head, vector<int> img)
{
	myImg image;

	image.head = head;
	image.img = img;

	imgQueue.push(image);

	ReleaseSemaphore(hsImageReady, 1, NULL);
}

bool imgBuffer::popBuffer(imgHead & head, vector<int> & img)
{
	if (!isBufEmpty())
	{
		head = imgQueue.front().head;

		img = imgQueue.front().img;

		imgQueue.pop();

		return true;
	}

	return false;
}

bool imgBuffer::isBufEmpty()
{
	return imgQueue.empty();
}

int imgBuffer::dropFrame(int cnt)
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