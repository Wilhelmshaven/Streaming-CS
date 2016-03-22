/*--Author������--*/

/*
	����ͷץȡģ�飺camCap.cpp, camCap.h

	���ã�
		ץȡ����ͷͼ��ת��ΪϵͳҪ��ĸ�ʽ�������û����õ�ʱ�򷵻�ͼ����һ��һά�������ʽ����

	˵����
		������һ��ģ�飬��������ϵͳû�й�ϵ�������ڹ���ͷ���治����OpenCV�Ķ������������������
*/
#pragma once
#include "CommonHeaders.h"

#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
using namespace cv;

/*
	����ͷ������

	ʹ�÷�����
	1.����startCapture()������
	2.����render()�������
	3.����getImage()��ȡͼ��
	4.����stopCapture()������

	�ӿڣ�

	void camCap::startCapture()����������ͷ��
	void camCap::stopCapture()���ر�����ͷ��

	void camCap::changeFrameRate(double frameRate)���ı�֡�ʣ���λΪÿ��֡��

	void camCap::showImg()������/�ر���Ƶ���ڣ�������أ������򿪡���ʼ״̬Ϊ������

	����ڷ�����static void render(char cmd = 0)������ָ����Ⱦͼ��
	������źţ�static HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(renderImage))

	�����ڷ�����Mat getImage()����ȡͼ��
	�������źţ�static HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(renderDone))
*/
class camCap
{
public:
	//����ͷ�����൥��
	static camCap *getInstance();

	//��������ͷ
	void startCapture();

	//�ر�����ͷ
	void stopCapture();

	//�ı�֡��
	void changeFrameRate(double frameRate);

	//�Ƿ���Ҫ�ۿ�����ͷͼ�����δ��ʾ����������ʾ������ȡ����ʾ
	void showImg();

	//����ָ����Ⱦͼ��
	static void render(char cmd = 0);

	//��ȡͼ��
	Mat getImage();

	~camCap();

private:

	//ͼ�����
	static queue<Mat> imgQueue;

	//����ָ����У����м��д��
	static queue<char> cmdQueue;

	//��ʼץȡ�¼�
	static HANDLE hEventStartCap;

	//�����߳��¼�	
	static HANDLE hEventShutDown;

	//չʾ֡�¼�
	static HANDLE hEventShowImg;

	//�̲߳����ṹ��
	typedef struct captureThreadParam
	{
		//�ȴ�ʱ�䣨֡�ʵĵ�������λ���룩
		int capRate;
	};
	captureThreadParam *capParam;

	/*
		ץȡ����ͷ�Ĵ����߳�
		������ͨ��captureThreadParam�ṹ�崫��
	*/
	static DWORD WINAPI captureThread(LPVOID lparam);
	
	/*
		����ģʽ���
	*/
	static camCap *instance;
	camCap();

	camCap(const camCap &);
	camCap &operator=(const camCap &);

	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo Garbo;
};