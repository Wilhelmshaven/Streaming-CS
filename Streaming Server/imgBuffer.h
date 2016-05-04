/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	图像缓存类（图像队列类）：单例类
	统一入口/出口：vector<unsigned char>

	为了减少转储，需要注意使用方法
	-->流媒体数据模块在接收时直接写入到缓存中，而播放器模块直接读取缓存

	使用：

	void pushBuffer(SOCKET index, imgHead head, shared_ptr<vector<BYTE>> img)：推入缓存

	bool popBuffer(SOCKET &index, imgHead &head, shared_ptr<vector<BYTE>> &img)：读取缓存头，返回是否成功

	bool isBufEmpty()：返回缓存是否为空

	//int dropFrame(unsigned int cnt)：丢弃掉指定数量的帧
*/
class imgBuffer
{
public:

	static imgBuffer* getInstance();

	void pushBuffer(SOCKET index, imgHead head, shared_ptr<vector<BYTE>> img);

	bool popBuffer(SOCKET &index, imgHead &head, shared_ptr<vector<BYTE>> &img);

	bool isBufEmpty();

	//int dropFrame(unsigned int cnt);

private:

	typedef struct myImg
	{
		SOCKET index;
		imgHead head;
		shared_ptr<vector<BYTE>> img;
	};

	queue<myImg> imgQueue;

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