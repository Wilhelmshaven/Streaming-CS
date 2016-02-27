/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	���е��м����д������
	������mw+����ģ�鵥�����

	�м���б�

*/

/*
	����������������������м��
	����Ҫ��֤����ģ�鶼��ʹ��ͬһ�����ݽ�����������Ҳ�ǵ���
*/
class mwPlayCtrl
{
public:
	
	static mwPlayCtrl* getInstance();

	void pushCtrlKey(char key);

	char getCtrlKey();

	~mwPlayCtrl();

private:

	queue<char> ctrlKeyQueue;

	static HANDLE hSemaphore;

	/*
		����ģʽ���
	*/

	static mwPlayCtrl *instance;

	mwPlayCtrl();

	mwPlayCtrl(const mwPlayCtrl &);
	mwPlayCtrl &operator=(const mwPlayCtrl &);

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