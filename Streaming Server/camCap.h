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
using namespace cv;

/*
	�̲߳����ṹ��
*/
typedef struct captureThreadParam
{
	//�ȴ�ʱ�䣨֡�ʵĵ�������λ���룩
	int capRate;
};

/*
	�¼�
*/
static HANDLE hEventStartCap;
static HANDLE hEventShutDown;    //�����߳��¼�	
static HANDLE hEventShowImg;     //չʾ֡�¼�

//��������ṹ��֡��
static Mat cvFrame;

/*
	����ͷ������

	ʹ�÷�����
	void camCap::startCapture()����������ͷ��
	void camCap::stopCapture()���ر�����ͷ��

	void camCap::changeFrameRate(double frameRate)���ı�֡�ʣ���λΪÿ��֡��

	int camCap::getHeight()���õ���ǰ֡�ĸ߶�
	int camCap::getWidth()���õ���ǰ֡�Ŀ��

	void camCap::writeBuf(vector<int> *vec)��д���棬����vector<int>���͵Ļ����Թ�д��

	void camCap::showImg()������/�ر���Ƶ���ڣ�������أ������򿪡���ʼ״̬Ϊ������
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

	//��ȡ֡�ĸ߶ȺͿ��
	int getHeight();
	int getWidth();

	//ȡ��ͼ��д�뻺��
	void writeBuf(vector<int> *vec);

	//�Ƿ���Ҫ�ۿ�����ͷͼ�����δ��ʾ����������ʾ������ȡ����ʾ
	void showImg();

	~camCap();

private:
	/*
		ץȡ����ͷ�Ĵ����߳�
		������ͨ��captureThreadParam�ṹ�崫��
	*/
	static DWORD WINAPI captureThread(LPVOID lparam);
	captureThreadParam *capParam;

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