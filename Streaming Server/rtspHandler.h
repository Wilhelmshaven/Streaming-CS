/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	NTP时间获取类

	使用：

	unsigned long getNTPTime()：返回NTP时间（距1900.1.1 00:00）

	unsigned long getNTPTimeUnix()：返回UNIX版NTP时间（距1970.1.1 00:00）
*/
class NTPTimeGenerator
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

	NTPTimeGenerator ntpTime;

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
	为了保证会话号唯一，如果同一秒内传入了多个连接，则其它连接使用从1开始的会话号
*/
class sessionGenerator
{
public:

	sessionGenerator();

	string getSessionID();

private:

	unsigned long session;

	unsigned long time;
	
	NTPTimeGenerator ntpTime;
};

/*
	RTSP消息处理类

	使用：

	void srvConfig(string URI = "", unsigned int srvPort = 8554)：服务器设置函数，目前只能设置连接使用的端口号

	string msgCodec(string msg)：编解码一体函数，输入客户端来的待解码信息，返回编码好的答复信息

	string getHandlerInfo()：获取rtsp处理器的相关信息，返回信息字符串

	string getErrMsg(int code)：错误信息翻译器，输入错误码，返回错误代码所表示的信息

	ps.
	编码依赖解码结果，这与客户端有着比较大的区别
	每个客户端的请求都不一样，所以要编解码一体（先这样吧，虽然我觉得还是可以拆开的）
	也可以考虑用工厂模式处理

	TODO：太多TODO了……
*/
class rtspHandler
{
public:

	static rtspHandler *getInstance();

	//设置服务器属性
	void srvConfig(string URI = "http://localhost:8554", unsigned int srvPort = 8554);

	//编解码一体，输入待解码信息，返回编码好的答复，不负责发送
	string msgCodec(SOCKET socket, string msg); 

	//获取rtsp处理器的相关信息
	string getHandlerInfo();         

private:

	typedef struct PerClientData
	{
		unsigned long session;

		SOCKET socket;

		unsigned int streamingPort;

		bool enableUDP;
	};

	//服务器RTSP方法
	vector<string> rtspMethod;                 
	
	//可用的服务器RTSP方法，与服务器RTSP方法数组对应
	vector<bool> availableMethod;              	

	/*
		处理器信息（其实就是流媒体服务器信息了）
	*/

	//流媒体地址（！！目前没用上，不知道填啥）
	string srvURI;   

	//RTSP版本
	string rtspVersion;  

	//服务器端口
	unsigned int srvPort;                      

	/*
		挂载其它小模块
	*/

	//SDP编码器
	sdpEncoder sdpHandler;                             

	//会话号生成器
	sessionGenerator sessGenerator;            

	/*
		封装编码过程中重复的代码段/功能
	*/

	//生成信令第一行
	string generateCMDLine(int errCode);

	//生成时间那一行
	string generateTimeLine();

	//截取第一段字符串
	string extractString(string &msg, bool newLine = false);

	//截取出会话号
	string extractSession(string msg);
	
	/*
		单例模式
	*/

	rtspHandler();

	static rtspHandler *instance;                  

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