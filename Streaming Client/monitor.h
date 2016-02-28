/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

#define MAX_CLOCK 1000

/*
	监控器模块，检测帧率，实时反馈给服务器（服务器管不管……就不知道了）
	毫无疑问，单例模式

	需要获知是否有超时的情况，请监控信号量hsMonitor

	使用：

	void initMonitor(int timingThreshold = 200)：设置超时阈值

	void beginTiming()：开始计时

	void endTiming()：结束计时

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