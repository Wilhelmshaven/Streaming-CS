/*--Author：李宏杰--*/
#pragma once
#include "CommonHeaders.h"

#include <stdlib.h>
#include <gl\glut.h>

typedef struct myImage
{
	imgHead head;

	shared_ptr<vector<BYTE>> imgData;
};

class glPlayer
{
public:

	static glPlayer* getInstance();

	void play();

	//入口：推入图像文件
	static void insertImage(imgHead head, shared_ptr<vector<BYTE>> image);

	//出口，弹出用户输入的按键
	static bool getCtrlKey(unsigned char &key);

private:

	//控制指令队列
	static queue<unsigned char> cmdQueue;

	//帧队列
	static queue<myImage> imgQueue;

	//从队列里获取帧
	static bool getImage(myImage &img);

	static void redraw();

	static void key(unsigned char k, int x, int y);

	static void idle();

	static void reshape(int width, int height);

	static DWORD WINAPI mainLoopThread(LPVOID lparam);

	static GLubyte* pPixelData;
	static GLubyte* prePixelData;

	/*
	单例模式相关
	*/

	glPlayer();

	static glPlayer *instance;

	glPlayer(const glPlayer &);
	glPlayer &operator=(const glPlayer &);

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
