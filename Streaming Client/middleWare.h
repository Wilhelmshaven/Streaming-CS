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

	void pushCtrlKey(char unicode)��������ģ�����м������Unicode��ʽ�İ���

	char getCtrlKey()�����������ģ�鷵��Unicode��ʽ�İ���

*/
class mwPlayCtrl
{
public:
	
	static mwPlayCtrl* getInstance();

	//ע�⣬����ΪUnicode
	void pushCtrlKey(char unicode);

	char getCtrlKey();

private:

	queue<char> ctrlKeyQueue;

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