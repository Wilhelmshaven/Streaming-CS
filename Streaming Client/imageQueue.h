/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

//OpenCV头
#include "opencv2\highgui\highgui.hpp"

using namespace cv;

/*
	图像缓存类（图像队列类）：单例类

	统一入口/出口：vector<int>，Mat
	客户端完成的是vector<int> -> Mat的工作
	服务端完成的是Mat -> vector<int>的工作

	为了减少转储，需要注意使用方法
	-->流媒体数据模块在接收时直接写入到缓存中，而播放器模块直接读取缓存

	使用：

	static imgBuffer* getInstance()：获取单例

	void pushBuffer(imgHead head, vector<int> img)：推入vector<int>类型到缓存中（同时完成类型转换）

	bool popBuffer(Mat &img)：读取缓存头，传入Mat指针供写入，返回是否成功

	bool isBufEmpty()：返回缓存是否为空
*/
class imgBuffer
{
public:

	static imgBuffer* getInstance();

	void pushBuffer(imgHead head, vector<int> img);

	bool popBuffer(Mat &img);

	bool isBufEmpty();

private:

	queue<Mat> imgQueue;	

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

//媒体缓存模块：标记有图像了，请取走播放
static HANDLE hsPlayBuffer = CreateSemaphore(NULL, 0, BUF_SIZE, NULL); 