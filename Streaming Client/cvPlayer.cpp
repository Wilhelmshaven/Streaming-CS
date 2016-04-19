/*--Author������--*/

#include "cvPlayer.h"

//���ھ�̬��Ա��������
HANDLE cvPlayer::heStart;
HANDLE cvPlayer::hePause;
HANDLE cvPlayer::heShutdown;

queue<unsigned char> cvPlayer::cmdQueue;
queue<shared_ptr<Mat>> cvPlayer::imgQueue;

int cvPlayer::frameRate;

//������ģ�飺ͼ���Ƿ�����
HANDLE hsPlayerInput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerInput);

//������ģ�飺��ȡ���˲�����֪ͨ�м��ȡ��
HANDLE hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

HANDLE heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::ESCPressed);

cvPlayer* cvPlayer::instance = new cvPlayer;

cvPlayer * cvPlayer::getInstance()
{
	return instance;
}

/*
	���캯��
	��ʼ���¼��������߳�
*/
cvPlayer::cvPlayer()
{
	frameRate = 50;

	heStart = CreateEvent(NULL, TRUE, FALSE, NULL);

	hePause = CreateEvent(NULL, TRUE, FALSE, NULL);

	heShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);

	CreateThread(NULL, NULL, playThreadFunc, NULL, NULL, NULL);
}

void cvPlayer::setFrameRate(unsigned short rate)
{
	frameRate = 1000 / rate;
}

void cvPlayer::play()
{
	SetEvent(heStart);

	ResetEvent(hePause);
}


void cvPlayer::pause()
{
	SetEvent(hePause);

	ResetEvent(heStart);
}

void cvPlayer::destroyPlayer()
{
	SetEvent(heShutdown);
	SetEvent(hePause);
	SetEvent(heStart);

	SetEvent(heCloseClient);
}


//��ԭvectorΪMat�����������
void cvPlayer::insertImage(imgHead head, shared_ptr<vector<BYTE>> image)
{
	//���vector�Ĵ�С���ԣ��ᱬ��CV�ڴ����
	Mat frame = Mat(*image).reshape(head.channels, head.yAxis.rows).clone();

	image.reset();

	frame.convertTo(frame, head.imgType);

	shared_ptr<Mat> ptr(new Mat);
	*ptr = frame;

	imgQueue.push(ptr);

	ReleaseSemaphore(hsPlayerInput, 1, NULL);
}

bool cvPlayer::getCtrlKey(unsigned char &key)
{
	if (cmdQueue.empty())
	{
		return false;
	}

	key = cmdQueue.front();

	cmdQueue.pop();

	return true;
}

bool cvPlayer::getImage(shared_ptr<Mat>& img)
{
	if (imgQueue.empty())
	{
		return false;
	}

	img = imgQueue.front();

	imgQueue.pop();

	return true;
}

DWORD cvPlayer::playThreadFunc(LPVOID lparam)
{
	/*
		����һ���հ�ͼ��������ʾ
		���Ȼ�ȡ��Ļ��С����󴴽�

		CV_[The number of bits per item][Signed or Unsigned][Type Prefix]C[The channel number]
		CV_8UC3��0-255���޷��ţ�3ͨ��

		����������ͼ��ÿ�����ض��ǣ�205,205,205���Ļ�ɫ
	*/

	Mat preFrame;

	int nScreenWidth, nScreenHeight;

	nScreenWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN) / 2;

	preFrame.create(nScreenHeight, nScreenWidth, CV_8UC3);

	string windowName = "Streaming Client Media Player";
	//namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	int key;

	shared_ptr<Mat> frame;

	while (1)
	{
		WaitForSingleObject(heStart, INFINITE);

		/*
			��ȡ����->���ţ�ѭ��
		*/

		while (1)
		{
			/*
				�ɹ���ȡ��ͼ���򲥷ţ��������˳��¼�
				��Ҫ����û���µ�һ֡����ʹ����һ֡�����Ż���
			*/
			if (WaitForSingleObject(hsPlayerInput, 0) == WAIT_OBJECT_0)
			{
				if (getImage(frame))
				{
					preFrame = *frame;
				}
			}

			imshow(windowName, preFrame);

			if (WaitForSingleObject(hePause, 0) == WAIT_OBJECT_0)
			{
				break;
			}

			//����֡�ʣ����ȴ����ܵ�����
			key = waitKey(frameRate);

			if (key != -1)
			{
				if (key == 27)
				{
					//ESC�������£��˳��ͻ���
					SetEvent(heCloseClient);
					
					break;
				}

				//���ư���Ƶ��
				Sleep(frameRate);

				//�����룬��֪ͨ�м����ȡ��
				cmdQueue.push(key);

				ReleaseSemaphore(hsPlayerOutput, 1, NULL);
			}

		}

		if (WaitForSingleObject(heCloseClient, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	
	return 0;
}