/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	日志模块

	主要功能有两个
	其一，记录错误信息到文本
	其二，记录运行数据到文本

	使用：

	void initLogModule(int queueCnt = N)：初始化模块，参数的意义在于获取性能数据（共N-1个时间点），不需要记录就无所谓了

	void logError(int errCode = -1, string error = "")：写入错误信息

	void insertTimestamp(int index, double timestamp)：写入时间戳到日志第index列中

	void shutdownModule()：关闭模块
*/
class logger
{
public:

	static logger *getInstance();

	static void initLogModule(int queueCnt = 9);

	static void logError(int errCode = -1, string error = "Unknown Error");

	static void insertTimestamp(int index, double timestamp);

	static void shutdownModule();

private:

	static vector<queue<double>> clockRecord;

	static int queueCnt;

	static fstream errFile;
	static fstream dataFile;

	//根据当天时间建立日志名前缀
	static void generatePath();

	//创建日志文件
	static void createLogFile(fstream &file, string filePath);

	/*
		徐行说：删除文件夹好用！
	*/

	static void silentlyRemoveDir(const char * _dir);
	static void silentlyRemoveDir(const wchar_t * _dir);

	/*
		错误信息预读：对于已有错误信息，先读进来
	*/

	/*
		单例模式相关
	*/

	logger();

	static logger *instance;

	logger(const logger &);
	logger &operator=(const logger &);

	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo Garbo;
};