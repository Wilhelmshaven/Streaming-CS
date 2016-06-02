#include "xRenderer.h"

/*
	����ͻ��˹��������������ֶ�ͻ��˾Ͳ�Ҫ����
	���ֵĻ����뱣��ʲô�ͻ������ԣ���ȥclientManager����Ľṹ����ӣ�Ȼ����addClient�����������ֵ
*/
#include "clientManager.h"

//��־
#include "logger.h"

//��ȡ���������ã���ʱ�����ڲ���Ҫ��������
//#include "monitor.h"

#include <cmath>

namespace xrNS
{
	//monitor *myCamClock = monitor::getInstance();
	logger *myCamLogger = logger::getInstance();

	//�򵥵�˵��������źŵƣ��а������ˣ��봦��
	HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));

	//�����źŵƣ����ͼ���Ѿ���Ⱦ�ã����м������
	HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));
};
using namespace xrNS;

queue<myImage> xRenderer::imgQueue;
queue<myCommand> xRenderer::cmdQueue;


xRenderer *xRenderer::getInstance()
{
	// xx
	// ע��ȫ�ֱ�����ʼ��˳��ԭ�������� new ������ָ�루ȫ�ֱ������������ﷵ�ص�ʱ��
	// ������Ϊ getInstance ���� new �����ã�����ȫ�ֱ����ĳ�ʼ�������� getInstance�������Ǹ�ȫ�ֱ����ĳ�ʼ��˳�����ǰ��
	// �����·��� nullptr
	// ��������Ӧ��������һʧ��д��

	static xRenderer ret;

	return &ret;
}

void xRenderer::startRenderer()
{
	//TODO: add your init codes

	rendererThread = thread([this]()
	{
		workerThread(0);
	});
}

void xRenderer::render(SOCKET index, unsigned char cmd)
{
	myCommand myCmd;

	myCmd.index = index;
	myCmd.key = cmd;

	cmdQueue.push(myCmd);

	ReleaseSemaphore(hsRenderImage, 1, NULL);
}

bool xRenderer::getImage(SOCKET &index, imgHead &head, vector<BYTE> &frame)
{
	if (imgQueue.empty())
	{
		return false;
	}

	// xx ���⿽����

	auto & ref = imgQueue.front();
	
	index = ref.index;
	head = ref.head;
	frame = move(ref.frame); // xx ���⿽����

	imgQueue.pop();

	return true;
}

DWORD WINAPI xRenderer::workerThread(LPVOID lparam)
{
	//...

	myImage image;

	//����һ�£�����socket����Ϊ�洢�ͻ�����Ϣ���������������к��������������
	SOCKET index;

	//Beep(1000, 100);

	while (1)
	{
		//Beep(2000, 100);

		//...

		//���а�����Ϣ���룬����ͻ�ȵ�
		WaitForSingleObject(hsRenderImage, INFINITE);
		//WaitForSingleObject(hsRenderImage, 1000);

		/*
			ȡ������
			��Ȼ��һ�δ���Ҳ�ǿ��Ը鵽����ģ����ֿܷ��ͻ��˵Ŀ�����
			low��low�ˣ���ֱ�Ӿ�����
		*/

		index = cmdQueue.front().index;

		BYTE inputKey = cmdQueue.front().key;

		cmdQueue.pop();

		/*
			TODO�����������
			������һ��������д�Ĵ���������ͼ����
			�Ҹ�����BYTE inputKey
			�㷵�ظ���һ��vector<BYTE> imageData

			������û��ͼ��ͷ
			���ܹ���֤�����imageData�ڲ�������ԭ����
			�������ݰ���ô����ô��
		*/

		//...

		image.frame.resize(640 * 480 * 4);

		for (size_t r = 0; r < 480; ++r)
			for (size_t c = 0; c < 640; ++c)
				for (size_t p = 0; p < 4; ++p)
				{
					image.frame[(r * 640 + c) * 4 + p] = rand();
				}
		

		/*
			�����ǰ�ͼ���������
			����дͼ��ͷ
			Ȼ����дSOCKET�ţ��ͻ��˱�ʶ��
			�����Ӳ��ͷ��ź���

			//TODO���Լ���ͼ��ͷ����
		*/

		image.head.cols = 640;    //��ͼ����
		image.head.rows = 480;    //��ͼ��߶�
		image.head.imgType = 0; //ͼ�����ͣ���㶨�壬�ͻ����ϵþͿ��ԣ����ÿ��Բ���
		image.head.channels = 0;//ͼ��ͨ��������㶨�壬�ͻ����ϵþͿ��ԣ����ÿ��Բ���

		image.index = index;

		imgQueue.push(move(image)); // xx�����⿽����

		ReleaseSemaphore(hsRenderDone, 1, NULL);
	}

	return 0;
}

xRenderer::xRenderer()
{

}