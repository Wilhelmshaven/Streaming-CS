/*--Author：李宏杰--*/
#include "glRenderer.h"

#include "monitor.h"

double fRotate = 0;
bool bAnim = false;
double eye[3] = { 0, 0, 8 };
double center[3] = { 0, 0, 0 };

//OpenGL搞不了1080p是什么节奏……C语言数组开不起那么大了是么
static const int glWidth = 480;
static const int glHeight = 480;

namespace glNS
{
	//摄标记中间件已拿到并转发解码好的指令，请渲染器（摄像头）处理
	HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));

	//标记图像已经渲染好，请中间件拿走
	HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

	HANDLE heStartPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::play);
	HANDLE heStopPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::stop);
	HANDLE hePausePlay = CreateEvent(NULL, TRUE, FALSE, syncManager::pause);

	
};
using namespace glNS;

queue<myImage> glRenderer::imgQueue;
queue<myCommand> glRenderer::cmdQueue;

glRenderer* glRenderer::instance = new glRenderer;

glRenderer * glRenderer::getInstance()
{
	return instance;
}

void glRenderer::startRenderer()
{
	CreateThread(NULL, NULL, mainLoopThread, NULL, NULL, NULL);
	//CreateThread(NULL, NULL, renderThread, NULL, NULL, NULL);
}

void glRenderer::render(SOCKET index, unsigned char cmd)
{
	myCommand myCmd;

	myCmd.index = index;
	myCmd.key = cmd;

	cmdQueue.push(myCmd);

	ReleaseSemaphore(hsRenderImage, 1, NULL);
}

bool glRenderer::getImage(SOCKET & index, imgHead & head, vector<BYTE>& frame)
{
	//myImage tmp;

	if (imgQueue.empty())
	{
		return false;
	}

	auto & tmp = imgQueue.front();
	
	index = tmp.index;
	head = tmp.head;
	frame = move(tmp.frame);

	imgQueue.pop();

	return true;
}

void glRenderer::redraw()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);				// 场景（0，0，0）的视点中心 (0,5,50)，Y轴向上

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_pos[] = { 5, 5, 5, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glEnable(GL_LIGHT0);

	glRotatef(fRotate, 0, 1.0f, 0);			// Rotate around Y axis
	glRotatef(-90, 1, 0, 0);
	glScalef(0.2, 0.2, 0.2);

	glPushMatrix();
	glTranslatef(-1.5, 0, 4 + 1);
	glRotatef(90, 1, 0, 0);
	glutSolidTeapot(1);
	glPopMatrix();

	if (bAnim) fRotate += 0.5f;

	glFlush();

	//glutSwapBuffers();
}

void glRenderer::reshape(int width, int height)
{
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	glOrtho(-3, 3, -3, 3, -100, 100);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void glRenderer::idle()
{
	glutPostRedisplay();
}

void glRenderer::key(unsigned char k, int x, int y)
{
	switch (k)
	{
	case 27:
	case 'q': {exit(0); break; }

	case ' ': {bAnim = !bAnim; break; }

	case 'a': {eye[0] = eye[0] + 0.2; center[0] = center[0] + 0.2;
		break;
	}
	case 'd': {eye[0] = eye[0] - 0.2; center[0] = center[0] - 0.2;
		break;
	}
	case 'w': {eye[1] = eye[1] - 0.2; center[1] = center[1] - 0.2;
		break;
	}
	case 's': {eye[1] = eye[1] + 0.2; center[1] = center[1] + 0.2;
		break;
	}
			  break;
	}
}

DWORD glRenderer::mainLoopThread(LPVOID lparam)
{
	int argc = 1;
	char *argv[1];

	argv[0] = "D:\Projects\Streaming-CS\Debug\Streaming Server.exe";

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);// | GLUT_DOUBLE);
	glutInitWindowSize(glWidth, glHeight);
	glutCreateWindow("Simple GLUT App");

	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	//glutIdleFunc(idle);

	glutIdleFunc([]()
	{
		idle();

		xRenderFunc(0);
	});

	glutMainLoop();

	return 0;
}

DWORD glRenderer::xRenderFunc(LPVOID lparam)
{
	vector<BYTE> vec;
	vec.resize(glWidth * glHeight * 4);

	myImage image;
	imgHead head;
	myCommand cmdStruct;

	BYTE myKey = 27;
	SOCKET index = 0;

	do
	{
		if (WaitForSingleObject(hsRenderImage, 0) == WAIT_OBJECT_0)
		{
			GLubyte* pPixelData = (GLubyte*)&vec[0];



			//取出控制信令
			if (!cmdQueue.empty())
			{
				cmdStruct = cmdQueue.front();
				cmdQueue.pop();

				myKey = cmdStruct.key;
				index = cmdStruct.index;
			}
			else
			{
				//102,Can't get decoded control msg from decodoer
				//myCamLogger->logError(102);

				continue;
			}

			key(myKey, 0, 0);

			if (myKey == 27 || myKey == 'q')break;

			//从OpenGL中截取数据
			glReadPixels(0, 0, glWidth, glHeight, GL_RGBA, GL_UNSIGNED_BYTE, pPixelData);

			auto error = glGetError();

			/*
				填充图像头
			*/

			head.cols = glWidth;
			head.rows = glHeight;
			head.channels = 4;
			head.imgType = GL_RGBA;

			image.head = head;
			image.index = index;
			image.frame = move(vec);

			//塞入队列
			imgQueue.push(move(image));

			ReleaseSemaphore(hsRenderDone, 1, NULL);
		}
	} while (0);

	return 0;
}

//DWORD glRenderer::renderThread(LPVOID lparam)
//{
//	vector<BYTE> vec;
//	vec.resize(480 * 480 * 4);
//
//	GLubyte* pPixelData = (GLubyte*)&vec[0];
//
//	myImage image;
//	imgHead head;
//	myCommand cmdStruct;
//
//	BYTE myKey;
//	SOCKET index;
//
//	while (1)
//	{
//		WaitForSingleObject(hsRenderImage, INFINITE);
//
//		//取出控制信令
//		if (!cmdQueue.empty())
//		{
//			cmdStruct = cmdQueue.front();
//			cmdQueue.pop();
//
//			myKey = cmdStruct.key;
//			index = cmdStruct.index;
//		}
//		else
//		{
//			//102,Can't get decoded control msg from decodoer
//			//myCamLogger->logError(102);
//
//			continue;
//		}
//
//		key(myKey, 0, 0);
//
//		if (myKey == 27 || myKey == 'q')break;
//
//		//从OpenGL中截取数据
//		glReadPixels(0, 0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, pPixelData);
//
//		auto error = glGetError();
//
//		/*
//			填充图像头
//		*/
//
//		head.cols = 1920;
//		head.rows = 1080;
//		head.channels = 4;
//		head.imgType = GL_RGBA;
//
//		image.head = head;
//		image.index = index;
//		image.frame = vec;
//
//		//塞入队列
//		imgQueue.push(image);
//
//		ReleaseSemaphore(hsRenderDone, 1, NULL);
//	}
//
//	return 0;
//}

glRenderer::glRenderer()
{

}