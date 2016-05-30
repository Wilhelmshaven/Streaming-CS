/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

#define MAX_CLOCK 1000

typedef struct timingClock
{
	LARGE_INTEGER startTime;
	LARGE_INTEGER endTime;
};

/*
	监控器模块，单例模式

	使用：

	void initMonitor(int frameRate = CLIENT_FRAME_RATE)：设置超时阈值，单位毫秒。注意，这个值的设置大于等于2000时失效。

	void beginTiming()：开始计时
	void endTiming()：结束计时

	void getTimeStamp(double &timeStamp)：直接获取时间戳
*/
class monitor
{
public:
	static monitor* getInstance();

	//设置帧率
	void initMonitor(unsigned int frameRate = CLIENT_FRAME_RATE);

	//开始计时
	void beginTiming();

	//结束计时
	void endTiming();

	//直接获取时间戳
	void getTimeStamp(double &timeStamp);

	//关闭计时器
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

	//帧率同步
	static unsigned int frameRate;

	static double timingThreshold;

	/*
		单例模式相关
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