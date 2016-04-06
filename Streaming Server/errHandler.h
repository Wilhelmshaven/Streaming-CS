#pragma once

#include "CommonHeaders.h"

/*
	һ���򵥵�ͨ�ô�������
	ĿǰҲ���Ǹ��ݴ����������Ļ�ϴ�ӡ��������Ϣ
	δ����������չ

	ʹ�ã�

	string handleError(unsigned int errCode)����������룬��Ļ�ϻ����������Ϣ��Ҳ���Դӷ���ֵ��ȡ
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
	RTSP������Ϣ������

	ʹ�ã�

	string getErrMsg(int code)����������룬���ش�����Ϣ
*/
class rtspErrHandler
{
public:

	static rtspErrHandler* getInstance();

	//���������Ϣ����������룬���ش�����Ϣ�ַ���
	string getErrMsg(int code);

private:

	//rtsp������Ϣ�ļ����ļ���ַ����Ե�ַ��  
	string settingFile;

	//�����������ӱ���ʱ�Ͳ��������ݲ��ϸ�ʽ��
	void buildErrList();

	//�������ӱ�Key-Value Map
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