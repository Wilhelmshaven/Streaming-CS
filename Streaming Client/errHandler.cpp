/*--Author：李宏杰--*/

#include "errHandler.h"

errHandler* errHandler::instance = new errHandler;

errHandler * errHandler::getInstance()
{
	return instance;
}

string errHandler::getErrorMsg(int errCode)
{
	string errMsg;

	auto iter = errList.find(errCode);

	if (iter == errList.end())
	{
		errMsg = "Unknown error.";
	}
	else
	{
		errMsg = iter->second;
	}

	cout << "Error: " << errMsg << endl;

	return errMsg;
}

void errHandler::buildErrList()
{
	fstream errFile;
	errFile.open(filePath, ios_base::in);

	string buf;

	if (!errFile.is_open())
	{
		cout << "建立系统错误信息表失败：表文件不存在" << endl;
	}
	else
	{
		/*
			成功打开文件，则一直读取到文件结束
			按行读取并解析
		*/
		int commaPos;
		int errCode;
		string errMsg;

		while (!errFile.eof())
		{
			getline(errFile, buf);

			//防止表尾空行报错
			if (buf.empty() || buf.find_first_not_of(' ') == string::npos)
			{
				break;
			}

			/*
				有内容，则解析之，先找逗号位置，前后分别处理
			*/

			commaPos = buf.find(',');

			errCode = stoi(buf.substr(0, commaPos), nullptr, 10);

			errMsg = buf.substr(commaPos + 1, buf.length() - commaPos);

			//插入Key-Value对
			errList.insert(make_pair(errCode, errMsg));
		}
	}
}

errHandler::errHandler()
{
	buildErrList();
}

rtspErrHandler * rtspErrHandler::getInstance()
{
	return instance;
}

/*----------------------RTSP错误信息处理器：RTSP 错误码信息查询---------------------*/
rtspErrHandler *rtspErrHandler::instance = new rtspErrHandler;

/*
	构造函数：写入文件名，建立错误列表
*/
rtspErrHandler::rtspErrHandler(string filePath)
{
	settingFile = filePath;

	buildErrList();
}

/*
	函数：建立RTSP错误信息表

	错误映射表用CSV格式存储
	CSV格式数据逗号分隔，非常方便，极其好扩展，直接EXCEL编辑即可
*/
void rtspErrHandler::buildErrList()
{
	//文件流对象
	fstream fileStream;

	//以只读方式打开文件
	fileStream.open(settingFile, ios_base::in);

	string buf, code, msg;
	int errCode;
	int commaPos;

	if (!fileStream.is_open())
	{
		cout << "建立RTSP错误信息表失败：表文件不存在" << endl;
	}
	else
	{
		/*
			成功打开文件，则一直读取到文件结束
			按行读取并解析
		*/
		while (!fileStream.eof())
		{
			getline(fileStream, buf);

			//已修复：如果表尾有空行，后面会报错
			if (buf.empty() || buf.find_first_not_of(' ') == string::npos)
			{
				break;
			}

			/*
				有内容，则解析之，先找逗号位置，前后分别处理
			*/

			commaPos = buf.find(',');

			errCode = stoi(buf.substr(0, commaPos), nullptr, 10);

			msg = buf.substr(commaPos + 1, buf.length() - commaPos);

			//插入Key-Value对
			errCodeList.insert(make_pair(errCode, msg));
		}
	}
}

/*
	输入错误码，通过查表(std::map)的方式返回错误信息字符串
*/
string rtspErrHandler::getErrMsg(int code)
{
	string msg;

	map<int, string>::iterator iter = errCodeList.find(code);

	if (iter != errCodeList.end())
	{
		msg = iter->second;
	}
	else
	{
		msg = "Invalid Error Code";
	}

	return msg;
}