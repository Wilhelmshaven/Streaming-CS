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

//ͳһ��ʼ��
xRenderer *xRenderer::instance = new xRenderer;

xRenderer *xRenderer::getInstance()
{
	return instance;
}

void xRenderer::startRenderer()
{
	//TODO: add your init codes

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
	myImage tmp;

	if (imgQueue.empty())
	{
		return false;
	}

	tmp = imgQueue.front();
	imgQueue.pop();

	index = tmp.index;
	head = tmp.head;
	frame = tmp.frame;

	return true;
}

DWORD WINAPI xRenderer::workerThread(LPVOID lparam)
{
	//...

	myImage image;
	imgHead imageHead;

	//����һ�£�����socket����Ϊ�洢�ͻ�����Ϣ���������������к��������������
	SOCKET index;

	vector<BYTE> imageData;

	while (1)
	{
		//...

		//���а�����Ϣ���룬����ͻ�ȵ�
		WaitForSingleObject(hsRenderImage, INFINITE);

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










		/*
			�����ǰ�ͼ���������
			����дͼ��ͷ
			Ȼ����дSOCKET�ţ��ͻ��˱�ʶ��
			�����Ӳ��ͷ��ź���

			//TODO���Լ���ͼ��ͷ����
		*/

		imageHead.cols = 0;    //��ͼ����
		imageHead.rows = 0;    //��ͼ��߶�
		imageHead.imgType = 0; //ͼ�����ͣ���㶨�壬�ͻ����ϵþͿ��ԣ����ÿ��Բ���
		imageHead.channels = 0;//ͼ��ͨ��������㶨�壬�ͻ����ϵþͿ��ԣ����ÿ��Բ���

		image.head = imageHead;
		image.index = index;

		image.frame = imageData;

		imgQueue.push(image);

		ReleaseSemaphore(hsRenderDone, 1, NULL);
	}

	return 0;
}

xRenderer::xRenderer()
{

}