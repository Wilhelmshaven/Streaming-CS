/*--Author������--*/

#include "middleWare.h"

mwPlayCtrl* mwPlayCtrl::instance = new mwPlayCtrl;

mwPlayCtrl * mwPlayCtrl::getInstance()
{
	return instance;
}

/*
	����������
	�����ź�����ʾ����һ��ģ����Ե���
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