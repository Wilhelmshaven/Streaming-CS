/*--Author������--*/

#include "myServer.h"

//�����м��
#include "middleWare.h"

/*
	ר�����ǰ����ʾ��Ϣ
*/
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     ����ʽ��ý������֡����ƽ̨���汾v1.0     =" << endl;
	cout << "=            143020085211001 ����            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--��ʼ����..." << endl;
	cout << "--��������..." << endl;
};
const myMessage myMsg;

namespace myServerNS
{
	//ȫ���¼����������������¼�
	HANDLE heSrvDown;
}
using namespace myServerNS;

/*
	��ʼ��������
*/
void initServer()
{
	heSrvDown = CreateEvent(NULL, TRUE, FALSE, TEXT(syncManager::srvDown));
}

//================================= MAIN =================================//
int main()
{
	initServer();

	middleWare *midWare = middleWare::getInstance();

	midWare->startMiddleWare();

	WaitForSingleObject(heSrvDown, INFINITE);

	/*
		ʣ��������
	*/

	return 0;
}