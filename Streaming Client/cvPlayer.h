/*--Author������--*/

/*
	����OpenCV�Ĳ�������ģ�飩

	ʹ��OpenCV��ԭ�򣺱Ƚ���Ϥ����д��ݣ�Ҳ�ܹ����׻�ȡ�������룬�Ҹò��ֱ������ں�������

	��ƣ�һ�����������Ӷ�����ȡ��ͼ�񲢲��ţ�ͬʱ��ȡ������Ϣ
*/

#pragma once
#include "CommonHeaders.h"

//����ͼ�񻺴�ģ��
#include "imageQueue.h"

#include "opencv2\highgui\highgui.hpp"

using namespace cv;

/*
	����OpenCV�Ĳ�����������ģʽ

	ʹ�ã�

	void play()������������

	static void setFrameRate(int frameRate = 20)��ָ��֡�ʣ���λΪ֡��ÿ��

	void shutdown()���رղ����������Կ���������

	void destroyPlayer()���ݻٲ�����������˼�壬�����Ժ�����Ҳ���������������ˣ�ֻ�������ͻ���

	char getCtrlKey()�������û�����İ���
*/
class cvPlayer
{
public:

	static cvPlayer* getInstance();

	void play();

	static void setFrameRate(int frameRate = 20);

	void shutdown();

	void destroyPlayer();

	static char getCtrlKey();

private:

	//����ָ�����
	static queue<char> cmdQueue;

	//֡�ʣ��������������֮֡�����ĺ���������֡�ʵĵ�����
	static int frameRate;

	/*
		�߳���أ��������϶���Ҫ�̵߳����
	*/

	//��ʼ�¼�
	static HANDLE hEventStart;

	//�����¼������Կ��ٿ�ʼ��
	static HANDLE hEventShutdown;

	//���׽����¼��������˳��̣߳�
	static HANDLE hEventDestroy;

	//�������߳�
	static DWORD WINAPI playThreadFunc(LPVOID lparam);

	/*
		����ģʽ���
	*/

	cvPlayer();

	static cvPlayer *instance;

	cvPlayer(const cvPlayer &);
	cvPlayer &operator=(const cvPlayer &);

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

//������ģ�飺��ȡ���˲�����֪ͨ�м��ȡ��
static HANDLE hsNewCtrlKey = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);