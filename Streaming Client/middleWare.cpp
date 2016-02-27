/*--Author：李宏杰--*/

#include "middleWare.h"

mwPlayCtrl* mwPlayCtrl::instance = new mwPlayCtrl;

mwPlayCtrl * mwPlayCtrl::getInstance()
{
	return instance;
}

/*
	向队列推入键，并增加信号量
*/
void mwPlayCtrl::pushCtrlKey(char key)
{
	ctrlKeyQueue.push(key);

	ReleaseSemaphore(hSemaphore, 1, NULL);
}

/*
	获取输入
	使用时，需要把他放在循环里定期检查，这里不负责定期检查
	调用方法后，检测信号量是否有，有则返回队列顶部的输入
*/
char mwPlayCtrl::getCtrlKey()
{
	if (WaitForSingleObject(hSemaphore, 0) == WAIT_OBJECT_0)
	{
		char key = ctrlKeyQueue.front();

		ctrlKeyQueue.pop();

		return key;
	}
	else
	{
		return NULL;
	}
}

mwPlayCtrl::~mwPlayCtrl()
{
	CloseHandle(hSemaphore);
}

mwPlayCtrl::mwPlayCtrl()
{
	//信号量第三个值在这里代表了控制队列的最大长度，谨慎设置
	hSemaphore = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);
}