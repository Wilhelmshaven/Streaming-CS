/*--Author������--*/

#include "middleWare.h"

mwPlayCtrl* mwPlayCtrl::instance = new mwPlayCtrl;

mwPlayCtrl * mwPlayCtrl::getInstance()
{
	return instance;
}

/*
	�������������������ź���
*/
void mwPlayCtrl::pushCtrlKey(char key)
{
	ctrlKeyQueue.push(key);

	ReleaseSemaphore(hSemaphore, 1, NULL);
}

/*
	��ȡ����
	ʹ��ʱ����Ҫ��������ѭ���ﶨ�ڼ�飬���ﲻ�����ڼ��
	���÷����󣬼���ź����Ƿ��У����򷵻ض��ж���������
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
	//�ź���������ֵ����������˿��ƶ��е���󳤶ȣ���������
	hSemaphore = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);
}