/*--Author������--*/

#include "camCap.h"

//���ھ�̬��Ա��������
HANDLE camCap::hEventStartCap;
HANDLE camCap::hEventShutDown;
HANDLE camCap::hEventShowImg;
Mat camCap::cvFrame;

/*
	��ʼ������ͷ����ģ��ĵ���
*/
camCap *camCap::instance = new camCap;

/*
	˽�й��캯��
*/
camCap::camCap()
{
	capParam = new captureThreadParam;
	capParam->capRate = 50;

	/*
		��ʼ���¼����ֹ�����ģʽ
	*/
	hEventStartCap = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShowImg = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShutDown = CreateEvent(NULL, TRUE, FALSE, NULL);

	/*
		�����߳�
	*/
	CreateThread(NULL, NULL, captureThread, capParam, NULL, NULL);
}

/*
	��������������ͷ
*/
void camCap::startCapture()
{
	//ʹ�ÿ�ʼ�¼������̵߳Ľ���
	SetEvent(hEventStartCap);

	//��ʾͼ��
	SetEvent(hEventShowImg);
}

/*
	�������ر�����ͷ
*/
void camCap::stopCapture()
{
	//ʹ�ÿ�ʼ�¼������̵߳Ľ���
	ResetEvent(hEventStartCap);

	//��λ��ʾͼ���¼�
	ResetEvent(hEventShowImg);
}

/*
	�������ı�֡��
	֡��ȡ��������Ϊץȡ��ʱ������
*/
void camCap::changeFrameRate(double frameRate)
{
	capParam->capRate = 1000 / frameRate;
}

/*
	����������֡��
*/
int camCap::getHeight()
{
	return cvFrame.rows;
}

/*
	����������֡��
*/
int camCap::getWidth()
{
	return cvFrame.cols;
}

/*
	����������ͨ����
*/
int camCap::getChannels()
{
	return cvFrame.channels();
}

/*
	���������ؾ�������
*/
int camCap::getType()
{
	return cvFrame.type();
}

/*
	������д�뻺��
	������
		��OpenCV��Mat�ṹ��ͼ��д�뵽�����һά�����

		Ŀǰ��������չ���ܣ�Ĭ�ϣ�ָ����֡����ͨ���ģ�ֵΪRGB��
		ֱ�Ӵ洢��һ�С�
*/
void camCap::writeBuf(vector<int> *vec)
{
	/*
		�����ظ��ƻ����ĺܴ��ʱ�䣬������Ҫ��ȡ�����ڴ渴�Ƶ���ʽ
		�������ǰѾ���תΪ��N*1�ľ�������һ���������ģ��Ͳ���Ҫ���������Լ����
		�����ԣ���ʱ��ĩ�Ჹ��һ���ļ�϶��������Ʃ����4����8�ı�����Ҫ���ڴ���룩
	*/
	*vec = cvFrame.reshape(1, 1);
}

/*
	������չʾͼ��
	������ͨ�������¼��������¼��ķ�ʽ���ı��߳��е����
*/
void camCap::showImg()
{
	if (WaitForSingleObject(hEventShowImg, 0) == WAIT_OBJECT_0)
	{
		ResetEvent(hEventShowImg);
	}
	else
	{
		SetEvent(hEventShowImg);
	}
}

camCap::~camCap()
{
	SetEvent(hEventShutDown);

	delete(capParam);

	CloseHandle(hEventStartCap);
	CloseHandle(hEventShowImg);
	CloseHandle(hEventShutDown);
}

camCap * camCap::getInstance()
{
	return instance;
}

/*
	�̴߳�����

	������
		���ȵȴ���ʼ�ź�
		Ȼ�󴴽���Ƶץȡ�ࣨ�͵�������ͷ�����ˣ�
		Ȼ�����һ������ѭ����
		������Ϊ��
			�������¼������������˳���
			������ͷ�����л�ȡ��ǰ֡��
			����Ƿ���Ҫ��ʾͼ��
			�ȴ�һ��ʱ������������֡�ʣ�
*/
DWORD WINAPI camCap::captureThread(LPVOID lparam)
{
	captureThreadParam *param = (captureThreadParam *)lparam;

	while (1)
	{
		//�ȴ���ʼ�ź�
		WaitForSingleObject(hEventStartCap, INFINITE);

		//����ͷ����
		VideoCapture capture(0);

		while (1)
		{
			//�������¼������ã�������߳�
			if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
			if (WaitForSingleObject(hEventStartCap, 0) != WAIT_OBJECT_0)break;

			//���豸��ȡ����ǰ֡
			capture >> cvFrame;

			//����ʾͼ���¼������ã�����ʾͼ��
			if (WaitForSingleObject(hEventShowImg, 0) == WAIT_OBJECT_0)
			{
				imshow("Camera Live!", cvFrame);
			}
			else
			{
				destroyWindow("Camera Live!");
			}

			//�ȴ�ʱ����
			waitKey(param->capRate);
		}

		//�������¼������ã�������߳�
		if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
	}

	delete(param);

	return 0;
}
