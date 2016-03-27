#pragma once

#include "CommonHeaders.h"

/*

	һ���򵥵Ĵ�������
	ĿǰҲ���Ǹ��ݴ����������Ļ�ϴ�ӡ��������Ϣ
	δ����������չ

	ʹ�ã�

	string handleError(unsigned int errCode)����������룬��Ļ�ϻ����������Ϣ��Ҳ���Դӷ���ֵ��ȡ

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