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

	ʹ�ã�

	startMiddleWare()�������м��

*/
class mwPlayCtrl
{
public:
	
	static mwPlayCtrl* getInstance();

	void startMiddleWare();

private:

	/*
		�̣߳���������������ʲô��
	*/

	static DWORD WINAPI mwCtrlMsgThread(LPVOID lparam);

	static DWORD WINAPI mwMediaThread(LPVOID lparam);

	/*
		����ģʽ���
	*/

	mwPlayCtrl();

	static mwPlayCtrl *instance;

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