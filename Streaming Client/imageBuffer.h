/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	ͼ�񻺴��ࣨͼ������ࣩ��������

	ʹ�ã�

	static imgBuffer* getInstance()����ȡ����

	void pushBuffer(imgHead head, shared_ptr<vector<BYTE>> img)�����뻺��

	bool popBuffer(imgHead &head, shared_ptr<vector<BYTE>> &img)����ȡ���棬�����Ƿ�ɹ�
*/
class imgBuffer
{
public:

	static imgBuffer* getInstance();

	//���뻺��
	void pushBuffer(imgHead head, shared_ptr<vector<BYTE>> img);

	//��ȡ���棬�����Ƿ�ɹ�
	bool popBuffer(imgHead &head, shared_ptr<vector<BYTE>> &img);

private:

	typedef struct myImage
	{
		imgHead head;
		shared_ptr<vector<BYTE>> img;
	};

	queue<myImage> imageQueue;	

	/*
		����Ϊ����ģʽ���
	*/

	imgBuffer();

	static imgBuffer *instance;

	//��ֹ���������Լ���ֵ
	imgBuffer(const imgBuffer &);
	imgBuffer & operator = (const imgBuffer &);

	//��������
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