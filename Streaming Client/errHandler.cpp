/*--Author������--*/

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

rtspErrHandler * rtspErrHandler::getInstance()
{
	return instance;
}

/*----------------------RTSP������Ϣ��������RTSP ��������Ϣ��ѯ---------------------*/
rtspErrHandler *rtspErrHandler::instance = new rtspErrHandler;

/*
	���캯����д���ļ��������������б�
*/
rtspErrHandler::rtspErrHandler(string filePath)
{
	settingFile = filePath;

	buildErrList();
}

/*
	����������RTSP������Ϣ��

	����ӳ�����CSV��ʽ�洢
	CSV��ʽ���ݶ��ŷָ����ǳ����㣬�������չ��ֱ��EXCEL�༭����
*/
void rtspErrHandler::buildErrList()
{
	//�ļ�������
	fstream fileStream;

	//��ֻ����ʽ���ļ�
	fileStream.open(settingFile, ios_base::in);

	string buf, code, msg;
	int errCode;
	int commaPos;

	if (!fileStream.is_open())
	{
		cout << "����RTSP������Ϣ��ʧ�ܣ����ļ�������" << endl;
	}
	else
	{
		/*
			�ɹ����ļ�����һֱ��ȡ���ļ�����
			���ж�ȡ������
		*/
		while (!fileStream.eof())
		{
			getline(fileStream, buf);

			//���޸��������β�п��У�����ᱨ��
			if (buf.empty() || buf.find_first_not_of(' ') == string::npos)
			{
				break;
			}

			/*
				�����ݣ������֮�����Ҷ���λ�ã�ǰ��ֱ���
			*/

			commaPos = buf.find(',');

			errCode = stoi(buf.substr(0, commaPos), nullptr, 10);

			msg = buf.substr(commaPos + 1, buf.length() - commaPos);

			//����Key-Value��
			errCodeList.insert(make_pair(errCode, msg));
		}
	}
}

/*
	��������룬ͨ�����(std::map)�ķ�ʽ���ش�����Ϣ�ַ���
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