#pragma once

#include "CommonHeaders.h"

typedef struct myImage
{
	imgHead head;

	shared_ptr<vector<BYTE>> imgData;
};

class xPlayer
{
public:

	static xPlayer* getInstance();

	//��ʼ���������о�д��д����Ҳ���ǲ�����
	void startPlayer();

	//��ڣ�����ͼ���ļ�
	static void insertImage(imgHead head, shared_ptr<vector<BYTE>> image);

	//���ڣ������û�����İ���
	static bool getCtrlKey(unsigned char &key);

private:

	//����ָ�����
	static queue<unsigned char> cmdQueue;

	//֡����
	static queue<myImage> imgQueue;

	//�������߳�
	static DWORD WINAPI playThreadFunc(LPVOID lparam);

	//�Ӷ������ȡ֡
	static bool getImage(myImage &img);

	/*
		����ģʽ���
	*/

	xPlayer();

	static xPlayer *instance;

	xPlayer(const xPlayer &);
	xPlayer &operator=(const xPlayer &);

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