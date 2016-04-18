/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	����������������������м��

	ʹ�ã�

	startMiddleWare()�������м��
*/
class middleWare
{
public:
	
	static middleWare* getInstance();

	void startMiddleWare();

	void shutdownAll();

private:

	void initHandles();

	/*
		�̣߳���������������ʲô��
		ע�⣬ĳЩ�ź�����������һ�����ޣ�̫���п��ܶ���
	*/

	static DWORD WINAPI mw_Player_Ctrl_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Ctrl_Net_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTSP_Net_RTSP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Net_RTP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTP_Buf_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Buf_Player_Thread(LPVOID lparam);

	//ר�ŷ���������
	//static DWORD WINAPI mw_HeartBeat(LPVOID lparam);

	static void sendRTSPMsg(int method);

	static void recvRTSPMsg(int &errCode);

	/*
		����ģʽ���
	*/

	middleWare();

	static middleWare *instance;

	middleWare(const middleWare &);
	middleWare &operator=(const middleWare &);
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