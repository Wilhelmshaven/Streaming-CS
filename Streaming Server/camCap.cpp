/*--Author������--*/

#include "camCap.h"

namespace camNS
{
	//����ͷ������м�����õ���ת������õ�ָ�����Ⱦ��������ͷ������
	HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));

	//����ͷ�����ͼ���Ѿ���Ⱦ�ã����м������
	HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

	HANDLE heStartPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::play);
	HANDLE heStopPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::stop);
	HANDLE hePausePlay = CreateEvent(NULL, TRUE, FALSE, syncManager::pause);

	//���ͻ��˲�����
	HANDLE hePlay = CreateEvent(NULL, TRUE, FALSE, NULL);
};

using namespace camNS;

//���ھ�̬��Ա��������
HANDLE camCap::hEventStartCap;
HANDLE camCap::hEventShutDown;
HANDLE camCap::hEventShowImg;

queue<myImage> camCap::imgQueue;
queue<myCommand> camCap::cmdQueue;

unsigned int camCap::capRate;
unsigned int camCap::frameRate;

camCap *camCap::instance = new camCap;

/*
	˽�й��캯��
*/
camCap::camCap()
{
	frameRate = 10;

	capRate = 1000/frameRate;

	/*
		��ʼ���¼����ֹ�����ģʽ
	*/
	hEventStartCap = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShowImg = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventShutDown = CreateEvent(NULL, TRUE, FALSE, NULL);

	/*
		�����߳�
	*/
	CreateThread(NULL, NULL, captureThread, NULL, NULL, NULL);
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
void camCap::changeFrameRate(unsigned int rate)
{
	frameRate = rate;

	capRate = 1000 / frameRate;
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

bool camCap::getImage(SOCKET &index, imgHead &head, vector<BYTE> &frame)
{
	myImage tmp;

	if (imgQueue.empty())
	{
		return false;
	}

	tmp = imgQueue.front();
	imgQueue.pop();
	
	index = tmp.index;
	head = tmp.head;
	frame = tmp.frame;

	return true;
}

int camCap::getFrameRate()
{
	return frameRate;
}

camCap::~camCap()
{
	SetEvent(hEventShutDown);

	Sleep(50);

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
	Mat cvFrame;

	myImage matStruct;
	myCommand cmdStruct;

	string windowTitle = "Camera Live!";

	SOCKET index;
	imgHead head;

	char key = 0;

	Size s;
	double scale = 1;

	//ѹ��������
	vector<int> compressParam(2);
	compressParam[0] = IMWRITE_JPEG_QUALITY;
	compressParam[1] = 100;

	while (1)
	{
		//�ȴ���ʼ�ź�
		WaitForSingleObject(hEventStartCap, INFINITE);

		//����ͷ����
		VideoCapture capture(0);
		int p = 0;
		while (1)
		{
			//�������¼������ã�������߳�
			if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
			if (WaitForSingleObject(hEventStartCap, 0) != WAIT_OBJECT_0)break;

			//���豸��ȡ����ǰ֡
			capture >> cvFrame;

			/*
				ȡ�����յ��Ŀͻ���ָ�����ָ���ͼ����һЩС�任/�ı�֡��

				TODO����װһ��
			*/

			if (WaitForSingleObject(hsRenderImage, 0) == WAIT_OBJECT_0)
			{	
				if (!cmdQueue.empty())
				{
					cmdStruct = cmdQueue.front();
					cmdQueue.pop();

					key = cmdStruct.key;
					index = cmdStruct.index;
				}

				switch (key)
				{
				/*
					����ͼ�����ű���
				*/
				case '1':
				{
					scale = 0.5;

					break;
				}
				case '2':
				{
					scale = 1;

					break;
				}
				case '3':
				{
					scale = 1.5;

					break;
				}
				case '4':
				{
					scale = 0.2;

					break;
				}

				/*
					����֡�ʱ仯
				*/
				case ',':
				{
					if (frameRate > 1)
					{
						//֡������Ҳ����1
						--frameRate;
					}

					changeFrameRate(frameRate);

					break;
				}
				case '.':
				{
					++frameRate;
					changeFrameRate(frameRate);

					break;
				}

				/*
					���Ʋ���ģʽ
				*/
				case 'p':
				{
					if (WaitForSingleObject(hePlay, 0) == WAIT_OBJECT_0)
					{
						ResetEvent(hePlay);
					}
					else
					{
						SetEvent(hePlay);
					}

					break;
				}

				default:
					break;
				}
			}	

			/*
				�������play״̬���򽫱任�õ�ͼ��ѹ���������뻺�棬���ͷ��ź���

				!!����û�д����ͻ����������ԭ�������ŷ��͵��������е�

				TODO�������ͻ���
			*/

			if ((WaitForSingleObject(hePlay, 0) == WAIT_OBJECT_0))
			{
				if (scale != 1)
				{
					resize(cvFrame, cvFrame, s, scale, scale);
				}

				////��ѹ��������
				//imgData = frame.reshape(1, 1);

				/*
					ѹ��ΪPNG
				*/
	
				imencode(".jpg", cvFrame, matStruct.frame, compressParam);

				/*
					���ͼƬͷ��
				*/

				head.channels = cvFrame.channels();
				head.cols = cvFrame.cols;
				head.rows = cvFrame.rows;
				head.imgType = cvFrame.type();

				/*
					���ṹ��
				*/

				matStruct.index = index;
				matStruct.head = head;

				imgQueue.push(matStruct);

				ReleaseSemaphore(hsRenderDone, 1, NULL);
			}

			//����ʾͼ���¼������ã�����ʾͼ��
			if (WaitForSingleObject(hEventShowImg, 0) == WAIT_OBJECT_0)
			{
				//��ʾ֡��
				putText(cvFrame, "Frame rate: " + to_string(frameRate), cvPoint(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255));

				imshow(windowTitle, cvFrame);
			}
			else
			{
				destroyWindow(windowTitle);
			}

			//�ȴ�ʱ����
			waitKey(capRate);
		}

		//�������¼������ã�������߳�
		if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
	}

	return 0;
}
