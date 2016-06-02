#pragma once

#include "CommonHeaders.h"

#include <thread>

/*
	��Щ���ǹ���������
	�Ҿ��ñ�ú�����ˣ��������ܾ��а�
	���˽������Կ�camCap.h/cpp��glRenderer.h/cpp
	������·

	��ʵ��ȻҪƵ���Խ�
	��ɻ�������Ǹ������Ⱑ
	������ÿ��һ���Ҿ�Ҫд��һ�Ѷ���
*/

typedef struct myImage
{
	myImage(){}
	// xx ʹ����ֵ���� move ���⿽��

	myImage(myImage && rr)
	{
		index = rr.index;
		head = rr.head;
		frame = move(rr.frame);

		rr.index = INVALID_SOCKET;
	}

	myImage & operator = (myImage && rr)
	{
		index = rr.index;
		head = rr.head;
		frame = move(rr.frame);

		rr.index = INVALID_SOCKET;
	}


	SOCKET index = INVALID_SOCKET;

	imgHead head;

	vector<BYTE> frame;
};

typedef struct myCommand
{
	SOCKET index;
	unsigned char key;
};

class xRenderer
{
public:
	static xRenderer *getInstance();

	//�ӿڣ�������Ⱦ�����ų�ʼ�����룬�Ź�����Ҳ�а����
	void startRenderer();

	//�̻��ӿڣ�����ָ����Ⱦͼ�񣬲���Ҫ�޸�
	static void render(SOCKET index, unsigned char cmd = 0);

	//�̻��ӿڣ���ȡͼ�񣬲���Ҫ�޸�
	bool getImage(SOCKET &index, imgHead &head, vector<BYTE> &frame);

private:

	//ͼ�����
	static queue<myImage> imgQueue;

	//����ָ����У����м��д��
	static queue<myCommand> cmdQueue;

	//����д��������
	DWORD WINAPI workerThread(LPVOID lparam);

	// xx
	//

	std::thread rendererThread;

	/*
		����ģʽ���
	*/

	static xRenderer *instance;
	xRenderer();

	xRenderer(const xRenderer &);
	xRenderer &operator=(const xRenderer &);

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