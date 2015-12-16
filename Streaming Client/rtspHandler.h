#include "CommonHeaders.h"

//RTSP������Ϣ������
class rtspErrHandler
{
public:
	rtspErrHandler();

	string getErrMsg(int code);              //���������Ϣ

private:
	string settingFile;
	void buildErrList();   //�����������ӱ��������ļ������ڻ������ݲ��ϸ�ʽ�ˣ�ֻ��Ϊ�˼��ٴ��������ѣ�Ҳ����

	map<int, string> errCodeList;
};

//RTSP��Ϣ�Ĵ������ҲҪ���ɵ���
//ԭ��ܼ򵥣���������ı����������Ҫ���ʵ����1���͹���
//��Σ����к���Ψһ�ģ�ÿ��һ������Ҫ����һ��
//�������Ҫ��ԭ�򣬷������������ˣ����ҲҪ����
//���仰˵�������ʵ�������ͷ�����һ��
class rtspHandler
{
public:
	static rtspHandler *getInstance();

	string getErrMsg(int code);      //���ش����������ʾ����Ϣ

private:
	static rtspHandler *instance;              //����
	rtspHandler();                             //���캯��

	int seqNum;                                //�������к�

	rtspErrHandler errHandler;    //������Ϣ������

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