/*--Author������--*/
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

	//��ڣ�����ͼ���ļ�
	static void insertImage(imgHead head, shared_ptr<vector<BYTE>> image);

	//���ڣ������û�����İ���
	static bool getCtrlKey(unsigned char &key);

private:

	//����ָ�����
	static queue<unsigned char> cmdQueue;

	//֡����
	static queue<myImage> imgQueue;

	//�Ӷ������ȡ֡
	static bool getImage(myImage &img);

	static void redraw();

	static void key(unsigned char k, int x, int y);

	static void idle();

	static void reshape(int width, int height);

	static DWORD WINAPI mainLoopThread(LPVOID lparam);

	static GLubyte* pPixelData;
	static GLubyte* prePixelData;

	/*
	����ģʽ���
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
