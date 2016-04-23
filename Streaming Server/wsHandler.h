/*--Author£ºÀîºê½Ü--*/
#pragma once

#include "CommonHeaders.h"

typedef struct myWSMsg
{
	SOCKET index;

	string msg;
};

/*
	Just a test for websocket.

	Abort.
*/
class wsHandler
{
public:

	static wsHandler* getInstance();

	static void decodeMsg(SOCKET socket, string msg);

	static bool getResponse(SOCKET &socket, string &msg);

private:

	static queue<myWSMsg> msgInQueue, msgOutQueue;

	static DWORD WINAPI decodeThreadFunc(LPVOID lapram);

	static string base64(const char* Data, int DataByte);

	/*
		µ¥Àý
	*/

	static wsHandler* instance;

	wsHandler();

	wsHandler(const wsHandler&);
	wsHandler &operator=(const wsHandler &);
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