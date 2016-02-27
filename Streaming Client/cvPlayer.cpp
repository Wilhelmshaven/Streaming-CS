/*--Author������--*/

#include "cvPlayer.h"

//�����м��
#include "middleWare.h"

//���ھ�̬��Ա��������
HANDLE cvPlayer::hEventStart;
HANDLE cvPlayer::hEventShutdown;
HANDLE cvPlayer::hEventDestroy;
int cvPlayer::frameRate;

cvPlayer* cvPlayer::instance = new cvPlayer;

cvPlayer * cvPlayer::getInstance()
{
	return instance;
}

void cvPlayer::play()
{
	SetEvent(hEventStart);
	ResetEvent(hEventShutdown);
}

void cvPlayer::setFrameRate(int rate)
{
	frameRate = 1000 / rate;
}

void cvPlayer::shutdown()
{
	SetEvent(hEventShutdown);
	ResetEvent(hEventStart);
}

void cvPlayer::destroyPlayer()
{
	SetEvent(hEventDestroy);
	SetEvent(hEventShutdown);
	SetEvent(hEventStart);
}

cvPlayer::~cvPlayer()
{

}

DWORD cvPlayer::playThreadFunc(LPVOID lparam)
{
	imgBuffer *imgBuf = imgBuffer::getInstance();

	mwPlayCtrl *midWare = mwPlayCtrl::getInstance();

	while (1)
	{
		WaitForSingleObject(hEventStart, INFINITE);

		/*
			����һ���հ�ͼ��������ʾ
			���Ȼ�ȡ��Ļ��С����󴴽�

			CV_[The number of bits per item][Signed or Unsigned][Type Prefix]C[The channel number]
			CV_8UC3��0-255���޷��ţ�3ͨ��

			����������ͼ��ÿ�����ض��ǣ�205,205,205���Ļ�ɫ
		*/
		Mat blankImg;

		int nScreenWidth, nScreenHeight;

		nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		blankImg.create(nScreenHeight, nScreenWidth, CV_8UC3);

		string windowName = "Streaming Client Media Player";
		namedWindow(windowName, CV_WINDOW_AUTOSIZE);

		imshow(windowName, blankImg);

		/*
			��ȡ����->���ţ�ѭ��
		*/

		Mat frame;

		while (1)
		{
			//�ɹ���ȡ��ͼ����������������˳��¼�
			if (imgBuf->popBuffer(frame))
			{
				imshow(windowName, frame);
			}

			if (WaitForSingleObject(hEventShutdown, 0) == WAIT_OBJECT_0)
			{
				break;
			}

			//����֡�ʣ����ȴ����ܵ�����
			int key = waitKey(frameRate);
			if (key != -1)
			{
				//�����룬ת��ָ�ͨ���м��ʵ��
				midWare->pushCtrlKey(key);
			}
		}

		if (WaitForSingleObject(hEventDestroy, 0) == WAIT_OBJECT_0)break;
	}
	
	return 0;
}

/*
	���캯��

	��Ҫ����
	��ʼ���¼��������߳�
*/
cvPlayer::cvPlayer()
{
	hEventStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventDestroy = CreateEvent(NULL, TRUE, FALSE, NULL);

	CreateThread(NULL, NULL, playThreadFunc, NULL, NULL, NULL);
}

