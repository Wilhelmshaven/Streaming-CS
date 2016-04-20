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

	void setFrameRate(int frameRate = 20)��ָ��֡�ʣ���λΪ֡��ÿ��

	void play()������������

	void pause()����ͣ������

	void destroyPlayer()���ݻٲ�����������˼�壬�����Ժ�����Ҳ���������������ˣ�ֻ�������ͻ���

	void insertImage(imgHead head, shared_ptr<vector<BYTE>> image)����ڣ�����ͼ���ļ�

	bool getCtrlKey(unsigned char &key)�����ڣ������û�����İ���

	int getFrameRate()����ȡ��ǰ֡��
*/
class cvPlayer
{
public:

	static cvPlayer* getInstance();

	//ָ��֡�ʣ���λΪ֡��ÿ��
	static void setFrameRate(unsigned short frameRate = 20);

	//����������
	void play();

	//��ͣ������
	void pause();

	//�ݻٲ�����������˼�壬�����Ժ�����Ҳ���������������ˣ�ֻ�������ͻ���
	void destroyPlayer();

	//��ڣ�����ͼ���ļ�
	static void insertImage(imgHead head, shared_ptr<vector<BYTE>> image);

	//���ڣ������û�����İ���
	static bool getCtrlKey(unsigned char &key);

	static int getFrameRate();

private:

	//����ָ�����
	static queue<unsigned char> cmdQueue;

	//֡����
	static queue<shared_ptr<Mat>> imgQueue;

	//֡�ʣ��������������֮֡�����ĺ���������֡�ʵĵ�����

	static int frameRate;
	static int playRate;

	//�Ӷ������ȡ֡
	static bool getImage(shared_ptr<Mat> &img);
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