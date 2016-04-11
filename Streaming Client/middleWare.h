/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	����������������������м��

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
		ע�⣬ĳЩ�ź�����������һ�����ޣ�̫���п��ܶ���
	*/

	static DWORD WINAPI mw_Player_Ctrl_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Ctrl_Net_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTSP_Net_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Net_RTSP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Net_RTP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTP_Buf_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Buf_Player_Thread(LPVOID lparam);

	//ר�ŷ���������
	//static DWORD WINAPI mw_HeartBeat(LPVOID lparam);

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