/*--Author������--*/

#include "cvPlayer.h"

#include "monitor.h"
#include "logger.h"

monitor *playerClock = monitor::getInstance();
logger *playerLogger = logger::getInstance();

//���ھ�̬��Ա��������
HANDLE cvPlayer::heStart;
HANDLE cvPlayer::hePause;
HANDLE cvPlayer::heShutdown;

queue<unsigned char> cvPlayer::cmdQueue;
queue<myImage> cvPlayer::imgQueue;

int cvPlayer::frameRate;
int cvPlayer::playRate;

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
	playRate = 10;
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


//��ԭvectorΪMat�����������
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

	int nScreenWidth, nScreenHeight;

	nScreenWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN) / 2;

	preFrame.create(nScreenHeight, nScreenWidth, CV_8UC3);

	string windowName = "Streaming Client Media Player";
	//namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	imshow(windowName, preFrame);
	int key;

	myImage img;

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
				if (getImage(img))
				{
					/*

					//��ѹ���Ĵ�����루��ԭMat��

					////���vector�Ĵ�С���ԣ��ᱬ��CV�ڴ����
					//Mat frame = Mat(*image).reshape(head.channels, head.yAxis.rows).clone();
					//frame.convertTo(frame, head.imgType);

					*/

					//���Դ���#5
					double timestamp5;
					playerClock->getTimeStamp(timestamp5);
					playerLogger->insertTimestamp(5, timestamp5);

					//��ѹ��ͼƬ
					Mat frame = imdecode(Mat(*(img.imgData)), CV_LOAD_IMAGE_COLOR);

					//���Դ���#6
					double timestamp6;
					playerClock->getTimeStamp(timestamp6);
					playerLogger->insertTimestamp(6, timestamp6);

					preFrame = frame;
				}
			

			imshow(windowName, preFrame);

			//���Դ���#7
			double timestamp7;
			playerClock->getTimeStamp(timestamp7);
			playerLogger->insertTimestamp(7, timestamp7);
			}
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
					++playRate;
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