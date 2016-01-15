#pragma once
#include "CommonHeaders.h"

//RTSP������Ϣ������
class rtspErrHandler
{
public:
	rtspErrHandler();

	string getErrMsg(int code);      //���������Ϣ

private:
	string settingFile;
	void buildErrList();             //�����������ӱ��������ļ������ڻ������ݲ��ϸ�ʽ�ˣ�ֻ��Ϊ�˼��ٴ��������ѣ�Ҳ����

	map<int, string> errCodeList;    //�������ӱ�
};

//RTSP��Ϣ�Ĵ���
//��������������������ͻ������űȽϴ������
//ÿ���ͻ��˵����󶼲�һ��������Ҫ�����һ��
class rtspHandler
{
public:
	static rtspHandler *getInstance();

	string getErrMsg(int code);      //���ش����������ʾ����Ϣ
	string getHandlerInfo();         //��ȡrtsp�������������Ϣ

	//ֻ�ܱ���룬��������
	string msgCodec(string msg);     //�����һ�壬�����������Ϣ�����ر���õĴ�

private:
	static rtspHandler *instance;              //����
	rtspHandler();                             //���캯��

	string URI;                                //��ý���ַ
	string rtspVersion;                        //RTSP�汾

	rtspErrHandler errHandler;                 //������Ϣ������

	vector<string> rtspMethod;                 //������rtsp����
	vector<bool> availableMethod;               //���õķ�����rtsp����

	//��ֹ���������Լ���ֵ
	rtspHandler(const rtspHandler &);
	rtspHandler & operator = (const rtspHandler &);

	//��������
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

