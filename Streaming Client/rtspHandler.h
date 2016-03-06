/*--Author：李宏杰--*/

#include "CommonHeaders.h"

/*
	rtspErrHandler类：RTSP错误信息翻译器

	描述：
	就是把rtsp的错误码转为字符串形式的错误信息；
	错误码和错误信息从文件读入，使用map形式存储；
	错误映射表用CSV格式存储，逗号分隔，非常方便，极其好扩展，直接EXCEL编辑即可；

	使用：
	string getErrMsg(int code)：输入错误码，返回字符串形式的错误信息
*/
class rtspErrHandler
{
public:

	rtspErrHandler();

	rtspErrHandler(string file);

	//输出错误信息
	string getErrMsg(int code);              

private:

	string settingFile;

	void buildErrList();   

	//错误链接表结构：<错误代码，错误信息>
	map<int, string> errCodeList;
};

/*
	rtspHandler类：RTSP消息的处理
	
	描述：
	单例。信令的编解码不需要多个实例，1个就够用。
	序列号是唯一的，每发一条就需要递增一次。
	服务器都单例了，这个也要单例。换句话说，这个的实例数量和服务器一样
	信令编解码以方法形式表现。注意：只管编解码，不管发送

	使用：

	（必须先设置）
	bool setHandler(string URI, WORD rtspVer, int port = 8554, bool enableUDP = false)：配置处理器：URI地址、RTSP版本、流媒体端口号、是否使用UDP

	string encodeMsg(int method)：流媒体信令编码，输入流媒体方法（常量，见公共头），返回字符串形式的信令

	int decodeMsg(string msg)：流媒体信令解码，输入为完整的流媒体信息，返回值为流媒体错误码（200为OK，其它为错误）
*/
class rtspHandler
{
public:
	static rtspHandler *getInstance();

	//配置处理器：URI地址、RTSP版本（默认1.0）、端口号（默认8554）、传输方法（默认TCP）
	bool setHandler(string URI, string rtspVersion = "1.0", int port = 8554, bool enableUDP = false);

	//编码信令，返回值为编码好的信息
	string encodeMsg(int method);   

	/*
		解码信令，返回值为错误代码，200为OK
		讲真，客户端其实只需要知道是否OK……
	*/
	int decodeMsg(string msg);     

	//输入RTSP错误码，返回其表示的错误信息
	string getErrMsg(int code);

	/*
		获取rtsp处理器的相关信息
		返回值示例："RTSP版本：1.0\r\n会话号（session）：FFFFFFFF\r\n端口号：8554\r\n传输方式：UDP\r\n"
	*/
	string getHandlerInfo();

private:

	//信令序列号
	int seqNum;   

	//客户端端口号，必须是偶数
	int streamingPort;

	//是否使用UDP：真则UDP，否则TCP
	bool enableUDP;

	//流媒体地址
	string URI;          

	//会话号
	string session;      

	//RTSP版本
	string rtspVersion;                        

	//轨道信息（目前不用）
	string trackNum;                           
	                           
	//rtsp错误信息处理器
	rtspErrHandler errHandler;                 

	/*
		单例模式相关
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