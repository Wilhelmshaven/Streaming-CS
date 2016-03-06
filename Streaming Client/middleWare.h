/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

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

	//ר�ŷ���������
	static DWORD WINAPI mwHeartBeat(LPVOID lparam);

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

//�м��������յ���RTSP�����Ƿ�OK
static HANDLE hsIsRTSPOK = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);

//�м��������Ƿ�ʼ����
static HANDLE heStartHeartBeat = CreateEvent(NULL, TRUE, FALSE, NULL);