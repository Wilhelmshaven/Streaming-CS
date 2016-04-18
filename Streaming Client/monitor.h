/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

#define MAX_CLOCK 1000

typedef struct timingClock
{
	LARGE_INTEGER startTime;
	LARGE_INTEGER endTime;
};

/*
	�����ģ�飬���֡�ʣ�ʵʱ���������������������ܲ��ܡ����Ͳ�֪���ˣ�
	�������ʣ�����ģʽ

	��Ҫ��֪�Ƿ��г�ʱ������������ź���hsMonitor

	ʹ�ã�

	void initMonitor(int timingThreshold = 200)�����ó�ʱ��ֵ����λ���롣ע�⣬���ֵ�����ô��ڵ���2000ʱʧЧ��

	void beginTiming()����ʼ��ʱ

	void endTiming()��������ʱ

*/
class monitor
{
public:
	static monitor* getInstance();

	//���ó�ʱ��ֵ����λ���롣ע�⣬���ֵ�����ô��ڵ���2000ʱʧЧ
	void initMonitor(int timingThreshold = 200);

	//��ʼ��ʱ
	void beginTiming();

	//������ʱ
	void endTiming();

	//�رռ�ʱ��
	void shutdown();

	~monitor();

private:

	static double frequency;

	static timingClock myClock[MAX_CLOCK];

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

//���ģ�飺����Ƿ��г�ʱ�����
static HANDLE hsTimeOut = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);