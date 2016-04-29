/*--Author������--*/

#include "monitor.h"

//������־��¼��
#include "logger.h"

//����������ȡ֡�ʣ����ڼ�¼
#include "cvPlayer.h"

monitor* monitor::instance = new monitor;

logger *myLog = logger::getInstance();
cvPlayer *myPlayer = cvPlayer::getInstance();

//���ģ�飺����Ƿ��г�ʱ�����
HANDLE hsTimeOut = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::timeOut);

double monitor::frequency;

//����һ��
timingClock monitor::myClock[MAX_CLOCK];

int monitor::startClockID;
int monitor::endClockID;
int monitor::timingThreshold;

HANDLE monitor::hSemaphoreBegin;
HANDLE monitor::hSemaphoreEnd;
HANDLE monitor::hEventShutdown;

monitor * monitor::getInstance()
{
	return instance;
}

void monitor::initMonitor(int threshold)
{
	timingThreshold = threshold + 100;

	if (timingThreshold > 2000)timingThreshold = 2000;
}

void monitor::beginTiming()
{
	ReleaseSemaphore(hSemaphoreBegin, 1, NULL);
}

void monitor::endTiming()
{
	ReleaseSemaphore(hSemaphoreEnd, 1, NULL);
}

void monitor::getTimeStamp(double &timeStamp)
{
	LARGE_INTEGER cnt;

	QueryPerformanceCounter(&cnt);

	timeStamp = (double)(cnt.QuadPart * 1000 / frequency);
}

void monitor::shutdown()
{
	SetEvent(hEventShutdown);
	ReleaseSemaphore(hSemaphoreBegin, 1, NULL);
	ReleaseSemaphore(hSemaphoreEnd, 1, NULL);
}

monitor::~monitor()
{
	CloseHandle(hSemaphoreBegin);
	CloseHandle(hSemaphoreEnd);
	CloseHandle(hEventShutdown);
}

/*
	����ʱ���ж��Ƿ�ʱ
	����ʹ���˱Ƚϸ߾��ȵļ�ʱ��������λ����

	TODO��
	�Լ�������һ���������⣬�����ȥ�޻��أ���ͣ���ʱ��Ϳ϶���ͣ����
*/
bool monitor::isTimeout(int frameRate, int clockID)
{
	double start, end, diff;

	start = (double)myClock[clockID].startTime.QuadPart;
	end = (double)myClock[clockID].endTime.QuadPart;

	diff = (end - start) * 1000 / frequency;

	if (diff > timingThreshold)
	{

#ifdef DEBUG
		cout << " Timeout!" << endl;
#endif // DEBUG

		diff = -1;

		return false;
	}

#ifdef DEBUG
	cout << "Delay: " << diff << "ms" << endl;
#endif // DEBUG

	string logMsg = to_string(frameRate) + ',' + to_string(diff);

	myLog->logData(logMsg);

	return true;
}

/*
	�߼������θ�ʱ�ӻ�ȡ��ʼʱ��
*/
DWORD monitor::beginTimingThreadFunc(LPVOID lparam)
{
	while (1)
	{
		WaitForSingleObject(hSemaphoreBegin, INFINITE);

		if (WaitForSingleObject(hEventShutdown, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		QueryPerformanceCounter(&(myClock[startClockID].startTime));

		++startClockID;
		if (startClockID == MAX_CLOCK)startClockID = 0;;
	}

	return 0;
}

/*
	�߼������θ�ʱ�ӻ�ȡ����ʱ�䣬�ж��Ƿ�ʱ����ʱ�������ź��������߳���ȥ�жϴ������緢���
*/
DWORD monitor::endTimingThreadFunc(LPVOID lparam)
{
	int frameRate;

	while (1)
	{
		WaitForSingleObject(hSemaphoreEnd, INFINITE);

		if (WaitForSingleObject(hEventShutdown, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		QueryPerformanceCounter(&(myClock[endClockID].endTime));

		frameRate = myPlayer->getFrameRate();

		if (isTimeout(frameRate, endClockID))
		{
			ReleaseSemaphore(hsTimeOut, 1, NULL);
		}

		++endClockID;
		if (endClockID == MAX_CLOCK)endClockID = 0;;
	}

	return 0;
}

monitor::monitor()
{
	hSemaphoreBegin = CreateSemaphore(NULL, 0, MAX_CLOCK, NULL);
	hSemaphoreEnd = CreateSemaphore(NULL, 0, MAX_CLOCK, NULL);

	hEventShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);

	CreateThread(NULL, NULL, beginTimingThreadFunc, NULL, NULL, NULL);
	CreateThread(NULL, NULL, endTimingThreadFunc, NULL, NULL, NULL);

	startClockID = 0;
	endClockID = 0;

	timingThreshold = 200;

	//��ȡϵͳʱ��Ƶ��

	LARGE_INTEGER freq;

	QueryPerformanceFrequency(&freq);

	frequency = (double)freq.QuadPart;
}