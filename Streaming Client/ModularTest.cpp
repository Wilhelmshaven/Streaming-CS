#include "CommonHeaders.h"

#include "myClient.h"

#include "opencv2\highgui\highgui.hpp"

#include "cvPlayer.h"

using namespace cv;

namespace myHandle
{
	HANDLE heCloseClient;

	HANDLE hsPlayerOutput;
};

using namespace myHandle;

void initServer()
{
	//这里还要仔细考虑，现在更多的是一次性程序的感觉。
	//TODO：增加复位功能
	heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::clientClose);      //事件：关闭客户端，初值为NULL

	hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);
}

int main()
{
	initServer();

	cvPlayer *camera = cvPlayer::getInstance();

	VideoCapture capture(0);

	Mat frame;

	camera->play();

	vector<char> img;

	imgHead head;

	while (1)
	{
		capture >> frame;

		string s;

		img = frame.reshape(1, 1);

		head.xAxis.cols = frame.rows;
		head.imgType = frame.type();
		head.channels = frame.channels();

		camera->insertImage(head, img);

		waitKey(50);

		if (WaitForSingleObject(hsPlayerOutput, 0) == WAIT_OBJECT_0)
		{
			char key;

			camera->getCtrlKey(key);

			cout << key << endl;
		}
	}

	return 0;
}