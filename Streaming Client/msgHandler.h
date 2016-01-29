/*
信令处理器
信令的结构固定，编解码等
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

//SDL控制信令编码器，嗯其实就是对固定结构的填充
class sdlCrtlMsgHandler
{
private:
	msgHead publicHead;
	sdlMsgHead sdlPublicHead;
	sdlKeyboardMsg keyboardMsg;
	sdlMouseMsg mouseMsg;

	int fillHead(int ctrlMsgType);  //填充信令头和SDL信令头，返回值为（？）

public:

	int fillPayload();   //这个看怎么设计？
};