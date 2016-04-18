/*--Author：李宏杰--*/

#include "monitor.h"

monitor* monitor::instance = new monitor;

double monitor::frequency;

//定义一下
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
	计算时间差，判断是否超时
	这里使用了比较高精度的计时方法，单位毫秒

	TODO：
	以及，还有一个严重问题，如果有去无回呢？那停表的时候就肯定会停乱了
*/
bool monitor::isTimeout(int clockID)
{
	double start, end, diff;

	start = (double)myClock[clockID].startTime.QuadPart;
	end = (double)myClock[clockID].endTime.QuadPart;

	diff = (end - start) * 1000 / frequency;

	if (diff > timingThreshold)
	{
		cout << " Timeout!" << endl;

		return false;
	}

	cout << "Delay: " << diff << "ms" << endl;

	return true;
}

/*
	逻辑：依次给时钟获取开始时间
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
	逻辑：依次给时钟获取结束时间，判断是否超时，超时则设置信号量（主线程再去判断处理，比如发信令）
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

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	frequency = (double)freq.QuadPart;
}