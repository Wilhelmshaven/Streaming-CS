/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	ͼ�񻺴��ࣨͼ������ࣩ��������

	ʹ�ã�

	static imgBuffer* getInstance()����ȡ����

	void pushBuffer(imgHead head, vector<unsigned char> img)�����뻺��

	bool popBuffer(imgHead &head, vector<unsigned char> &img)����ȡ���棬�����Ƿ�ɹ�
*/
class imgBuffer
{
public:

	static imgBuffer* getInstance();

	//���뻺��
	void pushBuffer(imgHead head, vector<unsigned char> img);

	//��ȡ���棬�����Ƿ�ɹ�
	bool popBuffer(imgHead &head, vector<unsigned char> &img);

private:

	typedef struct myImage
	{
		imgHead head;
		vector<unsigned char> img;
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