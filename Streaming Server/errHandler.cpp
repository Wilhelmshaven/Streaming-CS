#include "errHandler.h"

errHandler* errHandler::instance = new errHandler;

errHandler * errHandler::getInstance()
{
	return instance;
}

string errHandler::handleError(unsigned int errCode)
{
	string errMsg;

	auto iter = errList.find(errCode);

	if (iter == errList.end())
	{
		errMsg = "Unknown error...";
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