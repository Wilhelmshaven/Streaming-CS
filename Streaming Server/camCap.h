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
	����ͷ������

	ʹ�÷�����
	void camCap::startCapture()����������ͷ��
	void camCap::stopCapture()���ر�����ͷ��

	void camCap::changeFrameRate(double frameRate)���ı�֡�ʣ���λΪÿ��֡��

	int camCap::getHeight()���õ���ǰ֡�ĸ߶ȣ�������
	int camCap::getWidth()���õ���ǰ֡�Ŀ�ȣ�������
	int camCap::getChannels()����ȡ֡��ͨ����
	int camCap::getType()����ȡ֡�����ͣ����������ͣ����μ�cvdef.h��

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

	//��ȡ֡�ĸ߶ȣ��У�
	int getHeight();

	//��ȡ֡�Ŀ�ȣ��У�
	int getWidth();

	//��ȡ֡��ͨ����
	int getChannels();

	//��ȡ֡�����ͣ����������ͣ����μ�cvdef.h��
	int getType();

	//ȡ��ͼ��д�뻺��
	void writeBuf(vector<int> *vec);

	//�Ƿ���Ҫ�ۿ�����ͷͼ�����δ��ʾ����������ʾ������ȡ����ʾ
	void showImg();

	~camCap();

private:
	//��������ṹ��֡��
	static Mat cvFrame;

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