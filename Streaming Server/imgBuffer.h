/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	ͼ�񻺴��ࣨͼ������ࣩ��������
	ͳһ���/���ڣ�vector<unsigned char>

	Ϊ�˼���ת������Ҫע��ʹ�÷���
	-->��ý������ģ���ڽ���ʱֱ��д�뵽�����У���������ģ��ֱ�Ӷ�ȡ����

	ʹ�ã�

	void pushBuffer(imgHead head, vector<unsigned char> img)������vector<unsigned char>���͵�������

	bool popBuffer(imgHead &head, vector<unsigned char> &img)����ȡ����ͷ������imgHeadָ����vector<unsigned char>ָ�빩д�룬�����Ƿ�ɹ�

	bool isBufEmpty()�����ػ����Ƿ�Ϊ��

	int dropFrame(int cnt)��������ָ��������֡
*/
class imgBuffer
{
public:

	static imgBuffer* getInstance();

	void pushBuffer(imgHead head, vector<unsigned char> img);

	bool popBuffer(imgHead &head, vector<unsigned char> &img);

	bool isBufEmpty();

	int dropFrame(int cnt);

private:

	typedef struct myImg
	{
		imgHead head;
		vector<unsigned char> img;
	};

	queue<myImg> imgQueue;

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

