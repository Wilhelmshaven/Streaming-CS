/*--Author������--*/

#include "monitor.h"

//������־��¼��
#include "logger.h"

namespace monitorNS
{
	logger *myLog = logger::getInstance();

	//���ģ�飺����Ƿ��г�ʱ�����
	HANDLE hsTimeOut = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::timeOut);
}
using namespace monitorNS;

monitor* monitor::instance = new monitor;

double monitor::frequency;

timingClock monitor::myClock[MAX_CLOCK];

int monitor::startClockID;
int monitor::endClockID;
unsigned double monitor::timingThreshold;
unsigned int monitor::frameRate;

HANDLE monitor::hSemaphoreBegin;
HANDLE monitor::hSemaphoreEnd;
HANDLE monitor::hEventShutdown;

monitor * monitor::getInstance()
{
	return instance;
}

void monitor::initMonitor(unsigned int rate)
{
	timingThreshold = 1000 / rate;
	frameRate = rate;

	if (timingThreshold < 50)timingThreshold = 50;
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
*/
bool monitor::isTimeout(int clockID)
{
	double start, end, diff;

	start = (double)myClock[clockID].startTime.QuadPart;
	end = (double)myClock[clockID].endTime.QuadPart;

	diff = (end - start) * 1000 / frequency;

	if (diff > timingThreshold + 400)
	{
		cout << frameRate<<" "<<timingThreshold<<" "<<diff << endl;

		if (frameRate > 1)--frameRate;
		timingThreshold = 1000 / frameRate;

		return false;
	}

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
	while (1)
	{
		WaitForSingleObject(hSemaphoreEnd, INFINITE);

		if (WaitForSingleObject(hEventShutdown, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		QueryPerformanceCounter(&(myClock[endClockID].endTime));

		if (isTimeout(endClockID))
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