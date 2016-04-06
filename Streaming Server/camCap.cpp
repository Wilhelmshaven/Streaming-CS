/*--Author������--*/

#include "camCap.h"

//����ͷ������м�����õ���ת������õ�ָ�����Ⱦ��������ͷ������
HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));

//����ͷ�����ͼ���Ѿ���Ⱦ�ã����м������
HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

//���ھ�̬��Ա��������
HANDLE camCap::hEventStartCap;
HANDLE camCap::hEventShutDown;
HANDLE camCap::hEventShowImg;
queue<unsigned char> camCap::cmdQueue;
queue<Mat> camCap::imgQueue;

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

void camCap::render(unsigned char cmd)
{
	cmdQueue.push(cmd);

	ReleaseSemaphore(hsRenderImage, 1, NULL);
}

Mat camCap::getImage()
{
	Mat frame;

	if (!imgQueue.empty())
	{
		frame = imgQueue.front();
		imgQueue.pop();
	}

	return frame;
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

	Mat cvFrame;

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
				���յ��ͻ���ָ�������ر任���Ƴ�ͼ��
				ͨ���ź�������
			*/
			if (WaitForSingleObject(hsRenderImage, 0) == WAIT_OBJECT_0)
			{	
				char key = 0;
				if (!cmdQueue.empty())
				{
					key = cmdQueue.front();

					cmdQueue.pop();
				}

				/*
					����ָ���ͼ����һЩС�任

					TODO�����������Ϊһ��/�������
				*/
				switch (key)
				{
				case 'a':
				{
					Size s;
					resize(cvFrame, cvFrame, s, 0.5, 0.5);

					break;
				}
				case 'd':
				{
					Size s;
					resize(cvFrame, cvFrame, s, 1.5, 1.5);

					break;
				}
				default:
					break;
				}

				/*
					���任�õ�ͼ�����뻺�棬���ͷ��ź���
				*/

				imgQueue.push(cvFrame);
				ReleaseSemaphore(hsRenderDone, 1, NULL);

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
