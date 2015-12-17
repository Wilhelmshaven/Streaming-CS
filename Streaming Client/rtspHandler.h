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

	map<int, string> errCodeList;    //�������ӱ�
};

//RTSP��Ϣ�Ĵ������ҲҪ���ɵ���
//ԭ��ܼ򵥣���������ı���벻��Ҫ���ʵ����1���͹���
//��Σ����к���Ψһ�ģ�ÿ��һ������Ҫ����һ��
//�������Ҫ��ԭ�򣬷������������ˣ����ҲҪ����
//���仰˵�������ʵ�������ͷ�����һ��
class rtspHandler
{
public:
	static rtspHandler *getInstance();

	string getErrMsg(int code);      //���ش����������ʾ����Ϣ

	//ֻ�ܱ���룬��������
	string encodeMsg(int method);    //��������
	string decodeMsg(string msg);    //��������

	bool setHandler(WORD rtspVer, int port, bool enableUDP);           //���ô�������RTSP�汾���˿ںš����䷽��
	string getHandlerInfo();         //��ȡrtsp�������������Ϣ

private:
	static rtspHandler *instance;              //����
	rtspHandler();                             //���캯��

	int seqNum;                                //�������к�
	string session;                            //�Ự��
	WORD rtspVersion;                          //RTSP�汾
	int streamingPort;                         //�ͻ��˶˿ںţ�������ż��
	bool enableUDP;                            //ָ�����䷽��������UDP������TCP

	rtspErrHandler errHandler;                 //������Ϣ������

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