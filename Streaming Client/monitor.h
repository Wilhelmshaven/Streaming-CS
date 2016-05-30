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
	�����ģ�飬����ģʽ

	ʹ�ã�

	void initMonitor(int frameRate = CLIENT_FRAME_RATE)�����ó�ʱ��ֵ����λ���롣ע�⣬���ֵ�����ô��ڵ���2000ʱʧЧ��

	void beginTiming()����ʼ��ʱ
	void endTiming()��������ʱ

	void getTimeStamp(double &timeStamp)��ֱ�ӻ�ȡʱ���
*/
class monitor
{
public:
	static monitor* getInstance();

	//����֡��
	void initMonitor(unsigned int frameRate = CLIENT_FRAME_RATE);

	//��ʼ��ʱ
	void beginTiming();

	//������ʱ
	void endTiming();

	//ֱ�ӻ�ȡʱ���
	void getTimeStamp(double &timeStamp);

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

	static HANDLE hSemaphoreBegin;
	static HANDLE hSemaphoreEnd;

	static HANDLE hEventShutdown;

	//֡��ͬ��
	static unsigned int frameRate;

	static double timingThreshold;

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