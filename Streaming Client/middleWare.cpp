/*--Author：李宏杰--*/

#include "middleWare.h"

mwPlayCtrl* mwPlayCtrl::instance = new mwPlayCtrl;

mwPlayCtrl * mwPlayCtrl::getInstance()
{
	return instance;
}

/*
	向队列推入键
	调整信号量，示意下一个模块可以调用
*/
void mwPlayCtrl::pushCtrlKey(char unicode)
{
	ctrlKeyQueue.push(unicode);

	ReleaseSemaphore(hsMiddleWare, 1, NULL);
}

char mwPlayCtrl::getCtrlKey()
{
	char key;
	
	if (ctrlKeyQueue.empty())
	{
		key = NULL;
	}
	else
	{
		key = ctrlKeyQueue.front();
		ctrlKeyQueue.pop();
	}
		
	return key;	
}

mwPlayCtrl::mwPlayCtrl()
{

}