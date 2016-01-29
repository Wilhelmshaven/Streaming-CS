/*
�������
����Ľṹ�̶���������
*/
#pragma once
#include "CommonHeaders.h"

typedef struct msgHead
{
	WORD msgSize;
	BYTE payloadType;
	BYTE checksum;
};

typedef struct sdlMsgHead
{
	WORD msgSize;
	BYTE msgType;
	BYTE reserve;
};

typedef struct sdlKeyboardMsg
{
	WORD msgSize;
	BYTE msgType;
	BYTE reserve;

	BYTE isPressed;
	BYTE empty;
	WORD scancode;

	DWORD sdlKey;

	DWORD unicode;

	WORD sdlMode;
};

typedef struct sdlMouseMsg
{
	WORD msgSize;
	BYTE msgType;
	BYTE reserve;

	BYTE isPressed;
	BYTE mouseButton;
	BYTE mouseState;
	BYTE relativeMode;

	WORD pointerX;
	WORD pointerY;
};

//SDL�������������������ʵ���ǶԹ̶��ṹ�����
class sdlCrtlMsgHandler
{
private:
	msgHead publicHead;
	sdlMsgHead sdlPublicHead;
	sdlKeyboardMsg keyboardMsg;
	sdlMouseMsg mouseMsg;

	int fillHead(int ctrlMsgType);  //�������ͷ��SDL����ͷ������ֵΪ������

public:

	int fillPayload();   //�������ô��ƣ�
};