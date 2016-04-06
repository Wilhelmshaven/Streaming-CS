#pragma once

#include "CommonHeaders.h"

/*
	一个简单的通用错误处理器
	目前也就是根据错误代码在屏幕上打印出错误信息
	未来可随意扩展

	使用：

	string handleError(unsigned int errCode)：输入错误码，屏幕上会输出错误信息，也可以从返回值读取
*/
class errHandler
{
public:

	static errHandler* getInstance();

	string handleError(int errCode);

private:

	const string filePath = "config/static/systemErrCodeList.csv";

	void buildErrList();

	map<int, string> errList;

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

/*
	RTSP错误信息翻译器

	使用：

	string getErrMsg(int code)：输入错误码，返回错误信息
*/
class rtspErrHandler
{
public:

	static rtspErrHandler* getInstance();

	//输出错误信息：输入错误码，返回错误信息字符串
	string getErrMsg(int code);

private:

	//rtsp错误消息文件的文件地址（相对地址）  
	string settingFile;

	//建立错误链接表，暂时就不考虑内容不合格式了
	void buildErrList();

	//错误链接表：Key-Value Map
	map<int, string> errCodeList;

	/*
		Singleton
	*/

	static rtspErrHandler* instance;
	rtspErrHandler(string filePath = "config/static/rtspErrCodeList.csv");

	rtspErrHandler(const rtspErrHandler&);
	rtspErrHandler &operator=(const rtspErrHandler&);

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