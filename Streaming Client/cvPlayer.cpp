/*--Author������--*/

#include "cvPlayer.h"

/*
	ȫ�ֱ���
*/
//OpenCV����֡��
Mat frame;


cvPlayer *cvPlayer::instance = new cvPlayer;

cvPlayer * cvPlayer::getInstance()
{
	return instance;
}

int cvPlayer::buildPlayer()
{
	return 0;
}

int cvPlayer::playBuffer(vector<int> *img)
{
	Mat tmpFrame;



	return 0;
}

cvPlayer::~cvPlayer()
{
}

int cvPlayer::vecToMat(vector<int>& img)
{
	vector<int>::iterator iter, end;
	end = img.end();

	for (iter = img.begin(); iter < end; ++iter)
	{
		frame.push_back(iter);
	}
	

	return 0;
}
