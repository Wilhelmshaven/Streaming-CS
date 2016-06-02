#include "xRenderer.h"

/*
	加入客户端管理器：不想区分多客户端就不要管了
	区分的话，想保存什么客户端属性，就去clientManager里面的结构体里加，然后在addClient函数里面给初值
*/
#include "clientManager.h"

//日志
#include "logger.h"

//获取性能数据用，计时，现在不需要这玩意了
//#include "monitor.h"

#include <cmath>

namespace xrNS
{
	//monitor *myCamClock = monitor::getInstance();
	logger *myCamLogger = logger::getInstance();

	//简单点说就是入口信号灯：有按键来了，请处理
	HANDLE hsRenderImage = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));

	//出口信号灯：标记图像已经渲染好，请中间件拿走
	HANDLE hsRenderDone = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));
};
using namespace xrNS;

queue<myImage> xRenderer::imgQueue;
queue<myCommand> xRenderer::cmdQueue;


xRenderer *xRenderer::getInstance()
{
	// xx
	// 注意全局变量初始化顺序，原先在外面 new 并设置指针（全局变量），在这里返回的时候
	// 可能因为 getInstance 先于 new 被调用（其它全局变量的初始化调用了 getInstance，并且那个全局变量的初始化顺序更靠前）
	// 而导致返回 nullptr
	// 下面这种应该是万无一失的写法

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

	// xx 避免拷贝！

	auto & ref = imgQueue.front();
	
	index = ref.index;
	head = ref.head;
	frame = move(ref.frame); // xx 避免拷贝！

	imgQueue.pop();

	return true;
}

DWORD WINAPI xRenderer::workerThread(LPVOID lparam)
{
	//...

	myImage image;

	//解释一下，我用socket来作为存储客户端信息的主键，几乎所有函数都有这个参数
	SOCKET index;

	//Beep(1000, 100);

	while (1)
	{
		//Beep(2000, 100);

		//...

		//若有按键信息传入，这里就会等到
		WaitForSingleObject(hsRenderImage, INFINITE);
		//WaitForSingleObject(hsRenderImage, 1000);

		/*
			取出按键
			当然这一段代码也是可以搁到外面的，就能分开客户端的控制了
			low就low了，简单直接就行了
		*/

		index = cmdQueue.front().index;

		BYTE inputKey = cmdQueue.front().key;

		cmdQueue.pop();

		/*
			TODO：爱干嘛干嘛
			下面这一大块就是你写的处理按键给出图像了
			我给了你BYTE inputKey
			你返回给我一个vector<BYTE> imageData

			管你有没有图像头
			我能够保证无损把imageData在播放器还原给你
			具体数据爱怎么填怎么填
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
			这里是把图像送入队列
			先填写图像头
			然后填写SOCKET号（客户端标识）
			最后入队并释放信号量

			//TODO：自己填图像头就行
		*/

		image.head.cols = 640;    //填图像宽度
		image.head.rows = 480;    //填图像高度
		image.head.imgType = 0; //图像类型，随便定义，客户端认得就可以，不用可以不管
		image.head.channels = 0;//图像通道数，随便定义，客户端认得就可以，不用可以不管

		image.index = index;

		imgQueue.push(move(image)); // xx，避免拷贝！

		ReleaseSemaphore(hsRenderDone, 1, NULL);
	}

	return 0;
}

xRenderer::xRenderer()
{

}