#pragma once
#include "CommonHeaders.h"

//RTSP错误信息翻译器
class rtspErrHandler
{
public:
	rtspErrHandler();

	string getErrMsg(int code);      //输出错误信息

private:
	string settingFile;
	void buildErrList();             //建立错误链接表，不考虑文件不存在或者内容不合格式了，只是为了减少代码量而已，也不多

	map<int, string> errCodeList;    //错误链接表
};

//RTSP消息的处理
//编码依赖解码结果，这与客户端有着比较大的区别
//每个客户端的请求都不一样，所以要编解码一体
class rtspHandler
{
public:
	static rtspHandler *getInstance();

	string getErrMsg(int code);      //返回错误代码所表示的信息
	string getHandlerInfo();         //获取rtsp处理器的相关信息

	//只管编解码，不负责发送
	string msgCodec(string msg);     //编解码一体，输入待解码信息，返回编码好的答复

private:
	static rtspHandler *instance;              //单例
	rtspHandler();                             //构造函数

	string URI;                                //流媒体地址
	string rtspVersion;                        //RTSP版本

	rtspErrHandler errHandler;                 //错误信息处理器

	vector<string> rtspMethod;                 //服务器rtsp方法
	vector<bool> availableMethod;               //可用的服务器rtsp方法

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

