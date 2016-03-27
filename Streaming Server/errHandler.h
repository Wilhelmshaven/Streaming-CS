#pragma once

#include "CommonHeaders.h"

/*

	一个简单的错误处理器
	目前也就是根据错误代码在屏幕上打印出错误信息
	未来可随意扩展

	使用：

	string handleError(unsigned int errCode)：输入错误码，屏幕上会输出错误信息，也可以从返回值读取

*/
class errHandler
{
public:

	static errHandler* getInstance();

	string handleError(unsigned int errCode);

private:

	const string filePath = "config/static/systemErrCodeList.csv";

	void buildErrList();

	map<unsigned int, string> errList;

	//Singleton

	errHandler();

	static errHandler* instance;

	errHandler(const errHandler&);
	errHandler &operator=(const errHandler&);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo garbo;

};