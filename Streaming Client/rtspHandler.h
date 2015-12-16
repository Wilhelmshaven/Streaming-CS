#include "CommonHeaders.h"

//RTSP错误信息处理器
class rtspErrHandler
{
public:
	rtspErrHandler();

	string getErrMsg(int code);              //输出错误信息

private:
	string settingFile;
	void buildErrList();   //建立错误链接表，不考虑文件不存在或者内容不合格式了，只是为了减少代码量而已，也不多

	map<int, string> errCodeList;
};

//RTSP消息的处理，这个也要做成单例
//原因很简单，首先信令的编解码器不需要多个实例，1个就够用
//其次，序列号是唯一的，每发一条就需要递增一次
//最后最重要的原因，服务器都单例了，这个也要单例
//换句话说，这个的实例数量和服务器一样
class rtspHandler
{
public:
	static rtspHandler *getInstance();

	string getErrMsg(int code);      //返回错误代码所表示的信息

private:
	static rtspHandler *instance;              //单例
	rtspHandler();                             //构造函数

	int seqNum;                                //信令序列号

	rtspErrHandler errHandler;    //错误信息处理器

	//禁止拷贝构造以及赋值
	rtspHandler(const rtspHandler &);
	rtspHandler & operator = (const rtspHandler &);

	//析构处理
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