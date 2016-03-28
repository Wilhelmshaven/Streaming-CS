/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	图像缓存类（图像队列类）：单例类

	使用：

	static imgBuffer* getInstance()：获取单例

	void pushBuffer(imgHead head, vector<char> img)：推入缓存

	bool popBuffer(imgHead &head, vector<char> &img)：读取缓存，返回是否成功
*/
class imgBuffer
{
public:

	static imgBuffer* getInstance();

	void pushBuffer(imgHead head, vector<char> img);

	bool popBuffer(imgHead &head, vector<char> &img);

private:

	typedef struct myImage
	{
		imgHead head;
		vector<char> img;
	};

	queue<myImage> imageQueue;	

	/*
		以下为单例模式相关
	*/

	imgBuffer();

	static imgBuffer *instance;

	//禁止拷贝构造以及赋值
	imgBuffer(const imgBuffer &);
	imgBuffer & operator = (const imgBuffer &);

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