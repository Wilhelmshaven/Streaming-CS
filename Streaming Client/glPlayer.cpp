/*--Author：李宏杰--*/
#include "glPlayer.h"

double fRotate = 0;
bool bAnim = false;
double eye[3] = { 0, 0, 8 };
double center[3] = { 0, 0, 0 };

queue<unsigned char> glPlayer::cmdQueue;
queue<myImage> glPlayer::imgQueue;

GLubyte* glPlayer::pPixelData;
GLubyte* glPlayer::prePixelData;

namespace glNS
{
	//播放器模块：图像是否被输入
	HANDLE hsPlayerInput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerInput);

	//播放器模块：获取到了操作，通知中间件取走
	HANDLE hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

	HANDLE heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::ESCPressed);

	//超时
	HANDLE hsTimeOut = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::timeOut);
};
using namespace glNS;

glPlayer* glPlayer::instance = new glPlayer;

glPlayer * glPlayer::getInstance()
{
	return instance;
}

void glPlayer::play()
{
	CreateThread(NULL, NULL, mainLoopThread, NULL, NULL, NULL);
}

void glPlayer::insertImage(imgHead head, shared_ptr<vector<BYTE>> image)
{
	myImage img;
	img.head = head;
	img.imgData = image;

	imgQueue.push(img);

	ReleaseSemaphore(hsPlayerInput, 1, NULL);
}

bool glPlayer::getCtrlKey(unsigned char & key)
{
	if (cmdQueue.empty())
	{
		return false;
	}

	key = cmdQueue.front();

	cmdQueue.pop();

	return true;
}

bool glPlayer::getImage(myImage & img)
{
	if (imgQueue.empty())
	{
		return false;
	}

	img = imgQueue.front();

	imgQueue.pop();

	return true;
}

void glPlayer::redraw()
{
	myImage img;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//取出图像
	if (WaitForSingleObject(hsPlayerInput, 0) == WAIT_OBJECT_0)
	{
		if (getImage(img))
		{
			pPixelData = (GLubyte*)&(*img.imgData)[0];
			//glDrawPixels(img.head.xAxis.cols, img.head.yAxis.rows, img.head.imgType, GL_UNSIGNED_BYTE, pPixelData);

			//if (pPixelData != prePixelData)
			//{
			//	cout << 1 << endl;
			//	prePixelData = pPixelData;
			//}
		}
		else
		{
			//log error
		}
	}

	glRasterPos3d(1, 1, 1);
	glDrawPixels(480, 480, GLUT_RGB, GL_UNSIGNED_BYTE, prePixelData);

	//glutSwapBuffers();
	glFlush();
}

void glPlayer::key(unsigned char k, int x, int y)
{
	if (k == 27)
	{
		//ESC键被按下，退出客户端
		SetEvent(heCloseClient);
	}

	//有输入，则通知中间件来取走
	cmdQueue.push(k);

	ReleaseSemaphore(hsPlayerOutput, 1, NULL);
}

void glPlayer::idle()
{
	glutPostRedisplay();
}

void glPlayer::reshape(int width, int height)
{
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	glOrtho(-3, 3, -3, 3, -100, 100);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

DWORD glPlayer::mainLoopThread(LPVOID lparam)
{
	int argc = 1;
	char *argv[1];

	argv[0] = "D:\Projects\Streaming-CS\Debug\Streaming Client.exe";

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(480, 480);
	glutCreateWindow("Simple GLUT App");

	glutDisplayFunc(redraw);
	glutKeyboardFunc(key);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	vector<BYTE> vec;
	vec.resize(480 * 480 * 3);
	prePixelData = (GLubyte *)&vec[0];

	glutMainLoop();

	return 0;
}

glPlayer::glPlayer()
{

}