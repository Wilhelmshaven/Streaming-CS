/*--Author������--*/

/*
	����OpenCV�Ĳ�������ģ�飩

	ʹ��OpenCV��ԭ�򣺱Ƚ���Ϥ����д��ݣ�Ҳ�ܹ����׻�ȡ�������룬�Ҹò��ֱ������ں�������

	��ƣ�һ�����������Ӷ�����ȡ��ͼ�񲢲��ţ�ͬʱ��ȡ������Ϣ
*/

#pragma once
#include "CommonHeaders.h"

#include "opencv2\highgui\highgui.hpp"

using namespace cv;

/*
	����OpenCV�Ĳ�����������ģʽ

	ʹ�ã�

	static void setFrameRate(int frameRate = 20)��ָ��֡�ʣ���λΪ֡��ÿ��

	void play()������������

	void pause()����ͣ������

	void destroyPlayer()���ݻٲ�����������˼�壬�����Ժ�����Ҳ���������������ˣ�ֻ�������ͻ���

	static void insertImage(imgHead head, vector<char> image)����ڣ�����ͼ���ļ�

	static bool getCtrlKey(char &key)�����ڣ������û�����İ���
*/
class cvPlayer
{
public:

	static cvPlayer* getInstance();

	//ָ��֡�ʣ���λΪ֡��ÿ��
	static void setFrameRate(int frameRate = 20);

	//����������
	void play();

	//��ͣ������
	void pause();

	//�ݻٲ�����������˼�壬�����Ժ�����Ҳ���������������ˣ�ֻ�������ͻ���
	void destroyPlayer();

	//��ڣ�����ͼ���ļ�
	static void insertImage(imgHead head, vector<unsigned char> image);

	//���ڣ������û�����İ���
	static bool getCtrlKey(char &key);

private:

	//����ָ�����
	static queue<char> cmdQueue;

	//֡����
	static queue<Mat> imgQueue;

	//֡�ʣ��������������֮֡�����ĺ���������֡�ʵĵ�����
	static int frameRate;

	static bool getImage(Mat &img);

	/*
		�߳���أ��������϶���Ҫ�̵߳����
	*/

	//�������߳�
	static DWORD WINAPI playThreadFunc(LPVOID lparam);

	//��ʼ�¼�
	static HANDLE heStart;

	//��ͣ�¼�
	static HANDLE hePause;

	//���׽����¼��������˳��̣߳�
	static HANDLE heShutdown;

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