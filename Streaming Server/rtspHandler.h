/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	RTSP错误信息翻译器
	使用：
	string getErrMsg(int code)：输入错误码，返回错误信息
*/
class rtspErrHandler
{
public:
	rtspErrHandler();

	//输出错误信息：输入错误码，返回错误信息字符串
	string getErrMsg(int code);      

private:
	/*
		rtsp错误消息文件的文件地址（相对地址）
		TODO：可修改这里，以另行指定位置
	*/
	const string rtspErrFile = "config/static/rtspErrCodeList.csv";       

	string settingFile;
	void buildErrList();             //建立错误链接表，不考虑文件不存在或者内容不合格式了，只是为了减少代码量而已，也不多

	map<int, string> errCodeList;    //错误链接表
};

/*
	NTP时间获取类
	使用：
	unsigned long getNTPTime()：返回NTP时间（距1900.1.1 00:00）
	unsigned long getNTPTimeUnix()：返回UNIX版NTP时间（距1970.1.1 00:00）
*/
class NTPTime
{
public:
	unsigned long getNTPTime();
	unsigned long getNTPTimeUnix();
};

/*
	SDP编码器，SDP协议细节请参阅RFC2327文档：http://tools.ietf.org/html/rfc2327

	string encodeMsg()：编码，返回编码好的SDP信息

	TODO：太多……
*/
class sdpEncoder
{
public:
	string encodeMsg();

	sdpEncoder();

private:
	NTPTime ntpTime;
	string sdpMsg;

	//--Session description
	//Require
	string proVer;
	string owner;
	string sessionName;
	//Option
	string sessionInfo;
	string URI;
	string emailAddr;
	string phoneNum;
	string cnctInfo;
	string bandwidthInfo;
	string timeZoneAdjust;
	string encryptKey;
	string moreSessionAttr;

	//Time description
	string timeActive;     //require
	string repeatTime;     //option

	//Media description
	string mediaNameAndAddr;     //require
	string mediaTitle;
	//string cnctInfo;             //optional if included at session-level
	//string bandwidthInfo;
	//string encryptKey;
	//string moreSessionAttr;
};

/*
	一个随机Session号生成器

	string getSessionID()：返回一个会话号
	
	ps.
	气氛上我觉得直接用时间戳就好了……算了一下0xFFFFFFFF是足够的
	这当然有问题，详情请看实现部分
*/
class sessionGenerator
{
public:
	sessionGenerator();

	string getSessionID();

private:
	string session;
	
	NTPTime ntpTime;
};

//为每一个客户端存储对应数据
typedef struct PerClientData
{
	int streamingPort;
	bool enableUDP;
	int srvPort;
};

/*
	客户端列表类

	函数：
	int addClient(unsigned long session, PerClientData clientInfo)：插入客户端，输入会话号和客户端信息结构体

	unsigned long searchClient(unsigned long session)：查询客户端是否存在，输入会话号，返回会话号或0（不存在）

	PerClientData getClientInfo(unsigned long session)：获得客户端信息，输入会话号，返回客户端信息结构体

	int removeClient(unsigned long Session)：移除客户端，输入会话号，返回0
*/
class clientList
{
public: 

	//插入客户端
	int addClient(unsigned long session, PerClientData clientInfo);   
	
	//查询客户端是否存在
	unsigned long searchClient(unsigned long session); 
	
	//获得客户端信息
	PerClientData getClientInfo(unsigned long session);                

	//移除客户端
	int removeClient(unsigned long Session);                          
	 
	clientList();

private:
	map<unsigned long, PerClientData> client;
};

/*
	RTSP消息处理类

	函数：
	string getErrMsg(int code)：错误信息翻译器，输入错误码，返回错误代码所表示的信息

	string getHandlerInfo()：获取rtsp处理器的相关信息，返回信息字符串

	string msgCodec(string msg)：编解码一体函数，输入客户端来的待解码信息，返回编码好的答复信息

	int srvConfig(unsigned int srvPort)：服务器设置函数，目前只能设置连接使用的端口号

	ps.
	编码依赖解码结果，这与客户端有着比较大的区别
	每个客户端的请求都不一样，所以要编解码一体（先这样吧，虽然我觉得还是可以拆开的）

	TODO：太多TODO了……
*/
class rtspHandler
{
public:
	static rtspHandler *getInstance();

	string getErrMsg(int code);      //返回错误代码所表示的信息
	string getHandlerInfo();         //获取rtsp处理器的相关信息

	//只管编解码，不负责发送
	string msgCodec(string msg);     //编解码一体，输入待解码信息，返回编码好的答复

	//设置服务器属性
	int srvConfig(unsigned int srvPort);

private:
	string URI;                                //流媒体地址
	string rtspVersion;                        //RTSP版本
	unsigned int srvPort;                      //服务器端口

	rtspErrHandler errHandler;                 //错误信息处理器
	sdpEncoder sdpHandler;                     //SDP编码器

	vector<string> rtspMethod;                 //服务器rtsp方法
	vector<bool> availableMethod;              //可用的服务器rtsp方法

	clientList clientManager;                  //一个Session与客户端参数的对应表  
	sessionGenerator sessGenerator;            //会话号生成器

	/*
		单例模式
	*/
	static rtspHandler *instance;              //单例
	rtspHandler();                             //构造函数

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