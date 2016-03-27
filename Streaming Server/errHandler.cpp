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
		cout << "����ϵͳ������Ϣ��ʧ�ܣ����ļ�������" << endl;
	}
	else
	{
		/*
			�ɹ����ļ�����һֱ��ȡ���ļ�����
			���ж�ȡ������
		*/
		int commaPos;
		int errCode;
		string errMsg;

		while (!errFile.eof())
		{
			getline(errFile, buf);

			//��ֹ��β���б���
			if (buf.empty() || buf.find_first_not_of(' ') == string::npos)
			{
				break;
			}

			/*
				�����ݣ������֮�����Ҷ���λ�ã�ǰ��ֱ���
			*/

			commaPos = buf.find(',');

			errCode = stoi(buf.substr(0, commaPos), nullptr, 10);

			errMsg = buf.substr(commaPos + 1, buf.length() - commaPos);

			//����Key-Value��
			errList.insert(make_pair(errCode, errMsg));
		}
	}
}

errHandler::errHandler()
{
	buildErrList();
}