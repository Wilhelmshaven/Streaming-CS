/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	��Ϣ�м����������
	
	ʹ�ã�

	������ȵ���startMiddleWare()������ģ��
	�ر������ڷ������ر��¼�
*/
class middleWare
{
public:

	static middleWare* getInstance();

	void startMiddleWare();

private:

	void initHandles();

	static DWORD WINAPI mw_Cam_Buf_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Buf_RTP_Thread(LPVOID lparam);

	static DWORD WINAPI mw_RTP_Cnct_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Cnct_RTSP_Cnct_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Cnct_Ctrl_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Ctrl_Cam_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Cnct_Web_Thread(LPVOID lparam);

	static DWORD WINAPI mw_Web_Cnct_Thread(LPVOID lparam);

	/*
		����ģʽ
	*/

	middleWare();

	static middleWare* instance;
	
	middleWare(const middleWare&);
	middleWare &operator=(const middleWare&);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (instance)delete instance;
		}
	};
	static CGarbo garbo;

};