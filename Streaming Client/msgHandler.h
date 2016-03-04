/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	控制信令处理模块（在客户端，目前只负责编码）
	注意：目前不实现鼠标信令编码（对于毕设没必要）
	额，毫无疑问的，单例

	TODO：其它语言的键盘键位映射表

	使用：

	string keyboardMsgEncode()：编码键盘信令

	string mouseMsgEncode()：编码鼠标信令
*/

class msgHandler
{
public:

	static msgHandler* getInstance();

	string keyboardMsgEncode(char key);

	string mouseMsgEncode();

private:

	/*
		单例模式
	*/

	static msgHandler *instance;

	msgHandler();

	msgHandler(const msgHandler&);
	msgHandler &operator =(const msgHandler &);
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