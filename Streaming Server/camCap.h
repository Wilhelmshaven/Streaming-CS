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

typedef struct myImage
{
	SOCKET index;

	imgHead head;

	vector<BYTE> frame;
};

typedef struct myCommand
{
	SOCKET index;
	unsigned char key;
};

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

	void camCap::changeFrameRate(unsigned int frameRate)���ı�֡�ʣ���λΪÿ��֡��

	void camCap::showImg()������/�ر���Ƶ���ڣ�������أ������򿪡���ʼ״̬Ϊ������

	void render(SOCKET index, unsigned char cmd = 0)������ָ����Ⱦͼ��

	bool getImage(SOCKET &index, Mat frame)����ȡͼ��

	int getFrameRate()����ȡ֡��
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
	static void changeFrameRate(unsigned int frameRate);

	//�Ƿ���Ҫ�ۿ�����ͷͼ�����δ��ʾ����������ʾ������ȡ����ʾ
	void showImg();

	//����ָ����Ⱦͼ��
	static void render(SOCKET index, unsigned char cmd = 0);

	//��ȡͼ��
	bool getImage(SOCKET &index, imgHead &head, vector<BYTE> &frame);

	//��ȡ֡��
	static int getFrameRate();

	~camCap();

private:

	//ͼ�����
	static queue<myImage> imgQueue;

	//����ָ����У����м��д��
	static queue<myCommand> cmdQueue;

	//��ʼץȡ�¼�
	static HANDLE hEventStartCap;

	//�����߳��¼�	
	static HANDLE hEventShutDown;

	//չʾ֡�¼�
	static HANDLE hEventShowImg;

	//ץȡ����ͷ�Ĵ����߳�
	static DWORD WINAPI captureThread(LPVOID lparam);

	//֡�ʣ���λ����
	static unsigned int capRate;

	//֡��
	static unsigned int frameRate;
	
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