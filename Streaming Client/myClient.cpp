#include "CommonHeaders.h"
#include "rtspServer.h"
#include "myClient.h"

// ר�����ǰ����ʾ��Ϣ
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     ����ʽ��ý������֡����ƽ̨���汾v0.1     =" << endl;
	cout << "=            143020085211001 ����            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--��ʼ����..." << endl;
	cout << "--��ȡ��������Ϣ..." << endl;
};
const myMessage myMsg;

int main(int argc, char *argv[])
{

	rtspServer* myServer = rtspServer::getInstance();  //��ȡ��ý�������ʵ��


	return 0;
}