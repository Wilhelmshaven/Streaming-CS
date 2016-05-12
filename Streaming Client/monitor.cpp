/*--Author：李宏杰--*/

#include "monitor.h"

//加入日志记录器
#include "logger.h"

namespace monitorNS
{
	logger *myLog = logger::getInstance();

	//监控模块：标记是否有超时的情况
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
	计算时间差，判断是否超时
	这里使用了比较高精度的计时方法，单位毫秒
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

	//获取系统时钟频率

	LARGE_INTEGER freq;

	QueryPerformanceFrequency(&freq);

	frequency = (double)freq.QuadPart;
}