/*--Author������--*/

#include "myServer.h"

/*
	���ظ���ģ��
*/

//��������ģ�飬����֮
#include "cnctHandler.h"

//�����м��������֮
#include "middleWare.h"

//��������ͷ������֮
#include "camCap.h"

/*
	ר�����ǰ����ʾ��Ϣ
*/
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     ����ʽ��ý������֡����ƽ̨���汾v0.1     =" << endl;
	cout << "=            143020085211001 ����            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--��ʼ����..." << endl;
	cout << "--��������..." << endl;
};
const myMessage myMsg;

//================================= MAIN =================================//
int main(int argc, char *argv[])
{	
	/*------------------------------��������--------------------------*/
	
	//��������ģ��
	cnctHandler *networkModule = cnctHandler::getInstance();
	networkModule->startServer();

	//�����м��
	mwMsg *middleWare = mwMsg::getInstance();
	middleWare->startMiddleWare();

	//��������ͷ
	camCap *camera = camCap::getInstance();
	camera->startCapture();

	//TODO����������

	


	WaitForSingleObject(hSrvShutdown, INFINITE);

	//TODO��һϵ���ƺ���
	//delete networkModule;
	//delete middleWare;
	//delete camera;

	return 0;
}