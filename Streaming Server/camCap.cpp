/*--Author������--*/

#include "camCap.h"

//����ͼ�񻺴�ģ�飨�м����
#include "imageQueue.h"

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

	Sleep(50);

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

	imgBuffer *imgBuf = imgBuffer::getInstance();

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

			/*
				Important!�ǲ�ͣ�ķ��ͻ��ǿͻ��˲ٿغ�ŷ��ͣ�
				����VR��ģʽ���ͻ�û��ת���ӽ�ʱ��������ͣ���͵�
				����Ͱ������ʵ��
				���յ��ͻ���ָ����Ƴ�ͼ��
				ͨ���ź�������
			*/
			if (WaitForSingleObject(hsMsgHandler, 0) == WAIT_OBJECT_0)
			{
				imgBuf->pushBuffer(cvFrame);
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
