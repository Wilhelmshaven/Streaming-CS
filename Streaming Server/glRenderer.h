/*--Author：李宏杰--*/
#pragma once
#include "CommonHeaders.h"

#include <stdlib.h>
#include <gl\glut.h>

typedef struct myImage
{
	myImage(){}

	myImage(myImage && rr)
	{
		index = rr.index;
		head = rr.head;
		frame = move(rr.frame);

		rr.index = INVALID_SOCKET;
	}

	myImage & operator = (myImage && rr)
	{
		index = rr.index;
		head = rr.head;
		frame = move(rr.frame);

		rr.index = INVALID_SOCKET;
	}



	SOCKET index;

	imgHead head;

	vector<BYTE> frame;
};

typedef struct myCommand
{
	SOCKET index;
	unsigned char key;
};

class glRenderer
{
public:
	static glRenderer *getInstance();

	void startRenderer();

	//输入指令渲染图像
	static void render(SOCKET index, unsigned char cmd = 0);

	//获取图像
	bool getImage(SOCKET &index, imgHead &head, vector<BYTE> &frame);

private:

	//图像队列
	static queue<myImage> imgQueue;

	//控制指令队列，供中间件写入
	static queue<myCommand> cmdQueue;

	static void redraw();

	static void reshape(int width, int height);

	static void idle();

	static void key(unsigned char k, int x, int y);

	static DWORD WINAPI mainLoopThread(LPVOID lparam);

	static DWORD WINAPI renderThread(LPVOID lparam);

	static DWORD WINAPI xRenderFunc(LPVOID lparam);

	/*
		单例模式相关
	*/

	static glRenderer *instance;
	glRenderer();

	glRenderer(const glRenderer &);
	glRenderer &operator=(const glRenderer &);

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