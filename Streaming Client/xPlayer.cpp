#include "xPlayer.h"

#include "logger.h"

#include "monitor.h"

namespace xpNS
{
	monitor *playerClock = monitor::getInstance();
	logger *playerLogger = logger::getInstance();

	//������ģ�飺ͼ������
	HANDLE hsPlayerInput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerInput);

	//������ģ�飺��ȡ���˲�����֪ͨ�м��ȡ��
	HANDLE hsPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

	HANDLE heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::ESCPressed);

	//��ʱ
	//HANDLE hsTimeOut = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::timeOut);
}
using namespace xpNS;

queue<unsigned char> xPlayer::cmdQueue;
queue<myImage> xPlayer::imgQueue;

xPlayer* xPlayer::instance = new xPlayer;

xPlayer * xPlayer::getInstance()
{
	return instance;
}

void xPlayer::startPlayer()
{
	//TODO�������ĳ�ʼ�����룬�����Ҫ

}

void xPlayer::insertImage(imgHead head, shared_ptr<vector<BYTE>> image)
{
	myImage img;
	img.head = head;
	img.imgData = image;

	imgQueue.push(img);

	ReleaseSemaphore(hsPlayerInput, 1, NULL);
}

bool xPlayer::getCtrlKey(unsigned char & key)
{
	if (cmdQueue.empty())
	{
		return false;
	}

	key = cmdQueue.front();

	cmdQueue.pop();

	return true;
}

DWORD xPlayer::playThreadFunc(LPVOID lparam)
{
	imgHead imageHead;

	myImage img;

	BYTE inputKey;

	shared_ptr<vector<BYTE>> imageData;

	while (1)
	{
		if (WaitForSingleObject(hsPlayerInput, 0) == WAIT_OBJECT_0)
		{
			if (getImage(img))
			{
				//ȡ��ͼ������
				imageData = img.imgData;

				//ȡ��ͼ��ͷ
				imageHead = img.head;

				/*
					TODO����Ӳ��Ŵ���
					ͼ��������shared_ptr<vector<BYTE>> imageData��
					ͼ��ͷ��imgHead imageHead��
					����ԭ���������ˣ����Ի�ԭͼ���˰�
				*/







			}
		}

		if (1)          // TODO����Ϊif(�а������룩
		{
			//�����룬��֪ͨ�м����ȡ��
			cmdQueue.push(inputKey);

			ReleaseSemaphore(hsPlayerOutput, 1, NULL);
		}
	}

	/*
	//��Ȼ�����while���������滻Ϊ����ģ���Ϊ�����ڷ������

	while (1)
	{
		WaitForSingleObject(hsPlayerInput, INFINITE);
		
		if (getImage(img))
		{
			//ȡ��ͼ������
			imageData = img.imgData;

			//ȡ��ͼ��ͷ
			imageHead = img.head;

			/*
				TODO����Ӳ��Ŵ���
				ͼ��������shared_ptr<vector<BYTE>> imageData��
				ͼ��ͷ��imgHead imageHead��
				����ԭ���������ˣ����Ի�ԭͼ���˰�
			*//*





		}

		//Ȼ��������һ���̼߳�������ͺ���

		while (1)
		{
			if (1)          // TODO����Ϊif(�а������룩
			{
				//�����룬��֪ͨ�м����ȡ��
				cmdQueue.push(inputKey);

				ReleaseSemaphore(hsPlayerOutput, 1, NULL);
			}
		}
	}
	*/

	return 0;
}

bool xPlayer::getImage(myImage & img)
{
	if (imgQueue.empty())
	{
		return false;
	}

	img = imgQueue.front();

	imgQueue.pop();

	return true;
}

xPlayer::xPlayer()
{

}