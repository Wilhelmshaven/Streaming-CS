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

queue<myMat> camCap::imgQueue;
queue<myCommand> camCap::cmdQueue;

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

void camCap::render(SOCKET index, unsigned char cmd)
{
	myCommand myCmd;

	myCmd.index = index;
	myCmd.key = cmd;

	cmdQueue.push(myCmd);

	ReleaseSemaphore(hsRenderImage, 1, NULL);
}

bool camCap::getImage(SOCKET &index, Mat &frame)
{
	myMat tmp;

	if (imgQueue.empty())
	{
		return false;
	}

	tmp = imgQueue.front();
	imgQueue.pop();
	
	index = tmp.index;
	frame = tmp.frame;

	return true;
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

	myMat matStruct;
	myCommand cmdStruct;

	SOCKET index;

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
					cmdStruct = cmdQueue.front();
					cmdQueue.pop();

					key = cmdStruct.key;
					index = cmdStruct.index;
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
				case 'p':
				{
					Size s;
					resize(cvFrame, cvFrame, s, 0.2, 0.2);

					break;
				}
				default:
					break;
				}

				/*
					���任�õ�ͼ�����뻺�棬���ͷ��ź���
				*/

				matStruct.frame = cvFrame;
				matStruct.index = index;

				imgQueue.push(matStruct);

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
