/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

#define MAX_CLOCK 1000

/*
	�����ģ�飬���֡�ʣ�ʵʱ���������������������ܲ��ܡ����Ͳ�֪���ˣ�
	�������ʣ�����ģʽ

	��Ҫ��֪�Ƿ��г�ʱ������������ź���hsMonitor

	ʹ�ã�

	void initMonitor(int timingThreshold = 200)�����ó�ʱ��ֵ

	void beginTiming()����ʼ��ʱ

	void endTiming()��������ʱ

*/

typedef struct clock
{
	SYSTEMTIME beginTime;
	SYSTEMTIME endTime;
};


class monitor
{
public:
	static monitor* getInstance();

	void initMonitor(int timingThreshold = 200);

	void beginTiming();

	void endTiming();

	~monitor();

private:

	static clock myClock[MAX_CLOCK];

	static int startClockID;
	static int endClockID;

	static bool isTimeout(int clockID);

	static DWORD WINAPI beginTimingThreadFunc(LPVOID lparam);
	static DWORD WINAPI endTimingThreadFunc(LPVOID lparam);

	static int timingThreshold;

	static HANDLE hSemaphoreBegin;
	static HANDLE hSemaphoreEnd;

	static HANDLE hEventShutdown;

	/*
		����ģʽ���
	*/

	static monitor* instance;

	monitor();

	monitor(const monitor&);
	monitor &operator=(const monitor &);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	CGarbo garbo;
};