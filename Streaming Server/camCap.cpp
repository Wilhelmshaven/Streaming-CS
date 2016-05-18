/*--Author������--*/

#include "camCap.h"

//����ͻ��˹�����
#include "clientManager.h"

//��־
#include "logger.h"

//��ȡ���������ã���ʱ
//#include "monitor.h"

namespace camNS
{
	//monitor *myCamClock = monitor::getInstance();
	logger *myCamLogger = logger::getInstance();

	//����ͷ������м�����õ���ת������õ�ָ�����Ⱦ��������ͷ������
	HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));

	//����ͷ�����ͼ���Ѿ���Ⱦ�ã����м������
	HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

	HANDLE heStartPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::play);
	HANDLE heStopPlay = CreateEvent(NULL, TRUE, FALSE, syncManager::stop);
	HANDLE hePausePlay = CreateEvent(NULL, TRUE, FALSE, syncManager::pause);
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
	frameRate = cameraCaptureRate;

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
	CreateThread(NULL, NULL, ctrlDealingThread, NULL, NULL, NULL);
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
//void camCap::changeFrameRate(unsigned int rate)
//{
//	frameRate = rate;
//
//	capRate = 1000 / frameRate;
//}

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
	clientManager *clientList = clientManager::getInstance();

	Mat cvFrame, subFrame;

	myImage matStruct;

	string windowTitle = "Camera Live!";

	imgHead head;

	char key = 0;

	Size s;

	//ѹ��������
	vector<int> compressParam(2);
	compressParam[0] = IMWRITE_JPEG_QUALITY;
	compressParam[1] = 100;

	//������֡��
	unsigned signal = 0;

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

			++signal;
			//if (signal > 86400)signal = 1;

			/*
				�����ͻ����б�����Ӧ�Ĳ���������Ӧ��֡

				1.�ȼ���Ƿ񲥷ţ������Ų���
				2.�ټ��֡�ʣ�������������
				3.���Scale
				4.ѹ����ͳһѹ�������ˣ���ȻҲ������Ϊ�ͻ��˲�����һ���֣�
			*/

			auto iter = clientList->getIteratorStart();
			auto iterEnd = clientList->getIteratorEnd();

			while (iter != iterEnd)
			{
				if (!iter->second.play)
				{
					++iter;

					continue;
				}

				if (signal % (frameRate / iter->second.frameRate) != 0)
				{
					++iter;

					continue;
				}

				//if (iter->second.scaleFactor != 1)
				//{
					resize(cvFrame, subFrame, s, iter->second.scaleFactor, iter->second.scaleFactor);
				//}
				//else
				//{
				//	subFrame = cvFrame;
				//}

				/*
					����ͼ���ϼ�ˮӡ����ʽ��ʾ֡���Լ����������Ϣ
				*/

				putText(subFrame, "Frame rate: " + to_string(iter->second.frameRate), cvPoint(20, 20), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));
				putText(subFrame, "Resolution: " + to_string(subFrame.cols) + " X " + to_string(subFrame.rows), cvPoint(20, 35), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));

				/*
					���ͼƬͷ��
				*/

				head.channels = subFrame.channels();
				head.cols = subFrame.cols;
				head.rows = subFrame.rows;
				head.imgType = subFrame.type();

				/*
					���ṹ��
				*/

				matStruct.index = iter->second.socket;
				matStruct.head = head;

				////��ѹ��������
				//imgData = frame.reshape(1, 1);

				//ѹ��ΪJPG
				imencode(".jpg", subFrame, matStruct.frame, compressParam);

				imgQueue.push(matStruct);

				ReleaseSemaphore(hsRenderDone, 1, NULL);

				++iter;
			}

			/*
				�������ˣ�����ʾͼ���¼������ã�����ʾͼ��
			*/

			if (WaitForSingleObject(hEventShowImg, 0) == WAIT_OBJECT_0)
			{
				//��ʾ֡��
				putText(cvFrame, "Capture rate: " + to_string(frameRate), cvPoint(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255));

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

DWORD camCap::ctrlDealingThread(LPVOID lparam)
{
	clientManager *clientList = clientManager::getInstance();

	myCommand cmdStruct;

	SOCKET index;
	char key = 0;

	double scale = 1;
	int frameRateOffset = 0;

	//�ȴ���ʼ�ź�
	WaitForSingleObject(hEventStartCap, INFINITE);

	while (1)
	{
		//�������¼������ã�������߳�
		if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
		if (WaitForSingleObject(hEventStartCap, 0) != WAIT_OBJECT_0)break;

		/*
			ȡ�����յ��Ŀͻ���ָ�����ָ���ͼ����һЩС�任/�ı�֡��
		*/

		WaitForSingleObject(hsRenderImage, INFINITE);
		
		if (!cmdQueue.empty())
		{
			cmdStruct = cmdQueue.front();
			cmdQueue.pop();

			key = cmdStruct.key;
			index = cmdStruct.index;
		}
		else
		{
			//102,Can't get decoded control msg from decodoer
			myCamLogger->logError(102);

			continue;
		}

		switch (key)
		{
			/*
				����ͼ�����ű���
			*/
		case '1':
		{
			scale = 0.5;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '2':
		{
			scale = 1;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '3':
		{
			scale = 1.5;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '4':
		{
			scale = 2;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '5':
		{
			scale = 2.5;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '6':
		{
			scale = 3;
			clientList->changePlayFactor(index, scale);

			break;
		}
		case '7':
		{
			scale = 0.2;
			clientList->changePlayFactor(index, scale);

			break;
		}

		/*
			����֡�ʱ仯
		*/
		case ',':
		{
			frameRateOffset = -1;
			clientList->changePlayFactor(index, frameRateOffset);

			break;
		}
		case '.':
		{
			frameRateOffset = 1;
			clientList->changePlayFactor(index, frameRateOffset);

			break;
		}

		/*
			���Ʋ���ģʽ
		*/
		case 'p':
		{
			clientList->changePlayFactor(index, true);

			break;
		}

		default:
			break;
		}

		//�������¼������ã�������߳�
		if (WaitForSingleObject(hEventShutDown, 0) == WAIT_OBJECT_0)break;
	}

	return 0;
}
