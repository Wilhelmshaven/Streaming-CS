#include "camCap.h"

camCap::camCap()
{
	capRate = 0;

	//初始化事件
	hEventShowImg = CreateEvent(NULL, TRUE, FALSE, NULL);

	hEventShutDown = CreateEvent(NULL, TRUE, FALSE, NULL);
}

void camCap::startCapture(double frameRate)
{
	capRate = 1000 / frameRate;

	if (WaitForSingleObject(hEventShutDown, 0) != WAIT_OBJECT_0)
	{
		CreateThread(NULL, NULL, captureThread, NULL, NULL, NULL);
	}
}

void camCap::changeFrameRate(double frameRate)
{
	capRate = 1000 / frameRate;
}

int camCap::getHeight()
{
	return frame.rows;
}

int camCap::getWidth()
{
	return frame.cols;
}

void camCap::writeBuf(vector<int>& vec)
{
	for (iter = frame.begin<Vec3b>(); iter < frame.end<Vec3b>(); ++iter)
	{
		for (int i = 0; i < 3; ++i)
		{
			vec.push_back((*iter)[i]);
		}
	}
}

void camCap::showImg()
{
	if (WaitForSingleObject(hEventShowImg, 0))
	{
		ResetEvent(hEventShowImg);
	}
	else
	{
		SetEvent(hEventShowImg);
	}
}

camCap::~camCap()
{
	CloseHandle(hEventShowImg);
	CloseHandle(hEventShutDown);
}

DWORD WINAPI camCap::captureThread(LPVOID lparam)
{
	VideoCapture capture(0);

	while (1)
	{
		if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;

		capture >> frame;

		if (WaitForSingleObject(hEventShowImg, 0) == WAIT_OBJECT_0)
		{
			imshow("Camera Live!", frame);
		}

		waitKey(capRate);
	}

	return 0;
}
