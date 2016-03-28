/*--Author：李宏杰--*/

#pragma once
#include "CommonHeaders.h"

/*
	SETUP之后，RTP数据将通过用来发送RTSP命令的TCP Socket进行发送。RTP数据将以如下格式进行封装：

	| magic number | channel number | embedded data length | data |

	magic number - 1 byte value of hex 0x24
	RTP数据标识符，"$"
	channel number - 1 byte value to denote the channel
	信道数字 - 1个字节，用来指示信道
	embedded data length - 2 bytes to denote the embedded data length
	数据长度 - 2个字节，用来指示插入数据长度
	data - data packet, ie RTP packet, with the total length of the embedded data length
	数据 - 数据包，比如说RTP包，总长度与上面的数据长度相同
	Below is a full example of the communication exchanged

	随后，再附上通常的的RTP包，包括一个大头部
*/

/*
	RTP数据包处理类，负责解码与送入缓存

	ps.关于接收的情况，通常的做法是，信令一个套接字，媒体数据一个套接字
	但是上面是应用UDP传输的
	这里使用TCP的话，那么就是RTP over RTSP的传输方式
	即使用同一个套接字进行传输
	所以，接收统一由网络模块负责，若检测到是RTP数据包，则再转给本模块处理

	使用：

	void unpackRTP(string mediaPacket)：送入待解码的媒体数据包

	void getMedia(imgHead &head, vector<int> &img)：传出媒体数据

*/
class rtpHandler
{
public:

	static rtpHandler* getInstance();

	void unpackRTP(string mediaPacket);

	bool getMedia(imgHead &head, vector<char> &img);

private:

	typedef struct myImage
	{
		imgHead head;
		vector<char> img;
	};

	queue<myImage> imageQueue;

	/*
		单例模式相关
	*/
	rtpHandler();

	static rtpHandler* instance;

	rtpHandler(const rtpHandler&);
	rtpHandler &operator = (const rtpHandler&);
	class CGarbo
	{
	public:
		CGarbo()
		{
			if (instance)delete instance;
		}
	};
	CGarbo garbo;
};