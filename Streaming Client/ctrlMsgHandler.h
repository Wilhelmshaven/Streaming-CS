/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	控制信令处理模块（在客户端，目前只负责编码）
	注意：目前不实现鼠标信令编码（对于毕设没必要）
	额，毫无疑问的，单例

	TODO：其它语言的键盘键位映射表

	使用：

	void keyboardMsgEncode(char key)：编码键盘信令

	void mouseMsgEncode(char key)：编码鼠标信令

	bool getEncodedMsg(string &msg)：获取编码好的指令
*/

class ctrlMsgHandler
{
public:

	static ctrlMsgHandler* getInstance();

	void keyboardMsgEncode(char key);

	void mouseMsgEncode(char key);

	bool getEncodedMsg(string &msg);

private:

	queue<string> encodedMsgQueue;

	/*
		单例模式
	*/

	static ctrlMsgHandler *instance;

	ctrlMsgHandler();

	ctrlMsgHandler(const ctrlMsgHandler&);
	ctrlMsgHandler &operator =(const ctrlMsgHandler &);
	class CGarbo
	{
	public:
		CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo garbo;
};