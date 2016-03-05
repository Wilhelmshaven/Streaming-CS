/*--Author������--*/

#include "monitor.h"

monitor* monitor::instance = new monitor;

//����һ��
clock monitor::myClock[MAX_CLOCK];

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

monitor::~monitor()
{
	SetEvent(hEventShutdown);

	ReleaseSemaphore(hSemaphoreBegin, 1, NULL);
	ReleaseSemaphore(hSemaphoreEnd, 1, NULL);

	Sleep(50);

	CloseHandle(hSemaphoreBegin);
	CloseHandle(hSemaphoreEnd);
	CloseHandle(hEventShutdown);
}

//����ֻ�ж��˺����
bool monitor::isTimeout(int clockID)
{
	int diff = 0;
	int flag = false;

	clock tmp = myClock[clockID];

	//������ֵ����1�Ļ����ǲ������ˣ����ĳ���
	int s = tmp.endTime.wSecond - tmp.beginTime.wSecond;
	if (s > 1 || s < -1)
	{
		cout << " Timeout!" << endl;
		return flag;
	}

	diff += tmp.endTime.wMilliseconds - tmp.beginTime.wMilliseconds;
	if (diff < 0)diff += 1000;

	if (diff > timingThreshold)
	{
		flag = true;
		cout << " Timeout!" << endl;
	}
	else
	{
		cout << "Delay: " << diff << "ms" << endl;
	}

	return flag;
}

/*
	�߼������θ�ʱ�ӻ�ȡ��ʼʱ��
*/
DWORD monitor::beginTimingThreadFunc(LPVOID lparam)
{
	while (1)
	{
		WaitForSingleObject(hSemaphoreBegin, INFINITE);

		if (WaitForSingleObject(hEventShutdown, 0) == WAIT_OBJECT_0)break;

		GetSystemTime(&(myClock[startClockID].beginTime));

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

		if (WaitForSingleObject(hEventShutdown, 0) == WAIT_OBJECT_0)break;

		GetSystemTime(&(myClock[endClockID].endTime));

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
}