/*--Author������--*/

#include "cvPlayer.h"

#include "logger.h"

#include "monitor.h"

namespace cvPlayerNS
{
	monitor *playerClock = monitor::getInstance();
	logger *playerLogger = logger::getInstance();

	//������ģ�飺ͼ���Ƿ�����
	HANDLE hsPlayerInput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerInput);

	//������ģ�飺��ȡ���˲�����֪ͨ�м��ȡ��
	HANDLE hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

	HANDLE heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::ESCPressed);

	//��ʱ
	HANDLE hsTimeOut = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::timeOut);
}
using namespace cvPlayerNS;

cvPlayer* cvPlayer::instance = new cvPlayer;

//���ھ�̬��Ա��������
HANDLE cvPlayer::heStart;
HANDLE cvPlayer::hePause;
HANDLE cvPlayer::heShutdown;

queue<unsigned char> cvPlayer::cmdQueue;
queue<myImage> cvPlayer::imgQueue;

int cvPlayer::frameRate;
int cvPlayer::playRate;

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
	playRate = CLIENT_FRAME_RATE;
	frameRate = 1000 / playRate;

	heStart = CreateEvent(NULL, TRUE, FALSE, NULL);

	hePause = CreateEvent(NULL, TRUE, FALSE, NULL);

	heShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);

	CreateThread(NULL, NULL, playThreadFunc, NULL, NULL, NULL);
}

void cvPlayer::setFrameRate(unsigned short rate)
{
	playRate = rate;

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

void cvPlayer::insertImage(imgHead head, shared_ptr<vector<BYTE>> image)
{
	myImage img;
	img.head = head;
	img.imgData = image;

	imgQueue.push(img);

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

int cvPlayer::getFrameRate()
{
	return playRate;
}

bool cvPlayer::getImage(myImage &img)
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

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN) / 2;

	preFrame.create(nScreenHeight, nScreenWidth, CV_8UC3);

	string windowName = "Streaming Client Media Player";
	//namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	int key;

	myImage img;

	while (1)
	{
		WaitForSingleObject(heStart, INFINITE);

		/*
			��ȡ����->���ţ�ѭ��
		*/

		//playerClock->beginTiming();

		while (1)
		{
			/*
				�ɹ���ȡ��ͼ���򲥷ţ��������˳��¼�
				��Ҫ����û���µ�һ֡����ʹ����һ֡�����Ż���
			*/

			if (WaitForSingleObject(hsPlayerInput, 0) == WAIT_OBJECT_0)
			{
				/*
					����ʱ���
				*/

				//playerClock->endTiming();

				////��ʱ�ر���ֱ������С�ں�
				//if (WaitForSingleObject(hsTimeOut, 0) == WAIT_OBJECT_0)
				//{
				//	cmdQueue.push(',');
				//	
				//	setFrameRate(playRate - 1);

				//	ReleaseSemaphore(hsPlayerOutput, 1, NULL);
				//}

				//playerClock->beginTiming();

				if (getImage(img))
				{
					/*
					//��ѹ���Ĵ�����루��ԭMat��

					////���vector�Ĵ�С���ԣ��ᱬ��CV�ڴ����
					//Mat frame = Mat(*image).reshape(head.channels, head.yAxis.rows).clone();
					//frame.convertTo(frame, head.imgType);
					*/

					//��ѹ��ͼƬ
					Mat frame = imdecode(Mat(*(img.imgData)), CV_LOAD_IMAGE_COLOR);

					preFrame = frame;
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

				//���ư���Ƶ�ʣ�waitkey���º���������صģ����Եȴ�ʱ��᲻�㣬������Ҫ����һ��
				Sleep(frameRate);

				//�����룬��֪ͨ�м����ȡ��
				cmdQueue.push(key);

				ReleaseSemaphore(hsPlayerOutput, 1, NULL);

				if (key == ',')
				{
					if (playRate > 0)
					{
						--playRate;
					}
#ifdef DEBUG
					cout << "Frame Rate = " << playRate << endl;
#endif
				}

				if (key == '.')
				{
					if (playRate < 20)
					{
						++playRate;
					}
#ifdef DEBUG
					cout << "Frame Rate = " << playRate << endl;
#endif
				}
			}

		}

		if (WaitForSingleObject(heCloseClient, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	
	return 0;
}