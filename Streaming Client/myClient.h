#include "CommonHeaders.h"

//�������������ʼ��Ϣ
class myMessage
{
public:
	myMessage();
};

//���ﻹҪ��ϸ���ǣ����ڸ������һ���Գ���ĸо���
//TODO�����Ӹ�λ����
HANDLE hCloseClientEvent = CreateEvent(NULL, TRUE, FALSE, NULL);      //�¼����رտͻ��ˣ���ֵΪNULL
HANDLE hRTSPBeginEvent = CreateEvent(NULL, TRUE, FALSE, NULL);        //�¼�������RTSP�շ�
HANDLE hBeatStartEvent = NULL;                                        //�¼�������RTSP�����̣߳���ֵΪNULL