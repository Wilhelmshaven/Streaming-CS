#include "CommonHeaders.h"

//RTSP错误信息翻译器
class rtspErrHandler
{
public:
	rtspErrHandler();

	string getErrMsg(int code);              //输出错误信息

private:
	string settingFile;
	void buildErrList();   //建立错误链接表，不考虑文件不存在或者内容不合格式了，只是为了减少代码量而已，也不多

	map<int, string> errCodeList;    //错误链接表
};

//RTSP消息的处理，这个也要做成单例
//原因很简单，首先信令的编解码不需要多个实例，1个就够用
//其次，序列号是唯一的，每发一条就需要递增一次
//最后最重要的原因，服务器都单例了，这个也要单例
//换句话说，这个的实例数量和服务器一样
class rtspHandler
{
public:
	static rtspHandler *getInstance();

	string getErrMsg(int code);      //返回错误代码所表示的信息

	//只管编解码，不负责发送
	string encodeMsg(int method);    //编码信令
	string decodeMsg(string msg);    //解码信令

	bool setHandler(WORD rtspVer, int port, bool enableUDP);           //配置处理器：RTSP版本、端口号、传输方法
	string getHandlerInfo();         //获取rtsp处理器的相关信息

private:
	static rtspHandler *instance;              //单例
	rtspHandler();                             //构造函数

	int seqNum;                                //信令序列号
	string session;                            //会话号
	WORD rtspVersion;                          //RTSP版本
	int streamingPort;                         //客户端端口号，必须是偶数
	bool enableUDP;                            //指定传输方法：真则UDP，否则TCP

	rtspErrHandler errHandler;                 //错误信息处理器

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