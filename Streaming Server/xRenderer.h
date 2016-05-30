#pragma once

#include "CommonHeaders.h"

/*
	这些就是公共代码了
	我觉得标得很清楚了，反正能跑就行啊
	想了解更多可以看camCap.h/cpp和glRenderer.h/cpp
	都是套路

	其实既然要频繁对接
	抽成基类或许是个好主意啊
	不可能每换一次我就要写这一堆东西
*/

typedef struct myImage
{
	SOCKET index;

	imgHead head;

	vector<BYTE> frame;
};

typedef struct myCommand
{
	SOCKET index;
	unsigned char key;
};

class xRenderer
{
public:
	static xRenderer *getInstance();

	//接口：开启渲染器，放初始化代码，放构造里也行啊随便
	void startRenderer();

	//固化接口：输入指令渲染图像，不需要修改
	static void render(SOCKET index, unsigned char cmd = 0);

	//固化接口：获取图像，不需要修改
	bool getImage(SOCKET &index, imgHead &head, vector<BYTE> &frame);

private:

	//图像队列
	static queue<myImage> imgQueue;

	//控制指令队列，供中间件写入
	static queue<myCommand> cmdQueue;

	//代码写在这里面
	DWORD WINAPI workerThread(LPVOID lparam);

	/*
		单例模式相关
	*/

	static xRenderer *instance;
	xRenderer();

	xRenderer(const xRenderer &);
	xRenderer &operator=(const xRenderer &);

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