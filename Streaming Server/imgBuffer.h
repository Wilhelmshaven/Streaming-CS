/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	ͼ�񻺴��ࣨͼ������ࣩ��������
	ͳһ���/���ڣ�vector<int>

	Ϊ�˼���ת������Ҫע��ʹ�÷���
	-->��ý������ģ���ڽ���ʱֱ��д�뵽�����У���������ģ��ֱ�Ӷ�ȡ����

	ʹ�ã�

	void pushBuffer(imgHead head, vector<int> img)������vector<int>���͵�������

	bool popBuffer(imgHead &head, vector<int> &img)����ȡ����ͷ������imgHeadָ����vector<int>ָ�빩д�룬�����Ƿ�ɹ�

	bool isBufEmpty()�����ػ����Ƿ�Ϊ��

	int dropFrame(int cnt)��������ָ��������֡
*/
class imgBuffer
{
public:

	static imgBuffer* getInstance();

	void pushBuffer(imgHead head, vector<int> img);

	bool popBuffer(imgHead &head, vector<int> &img);

	bool isBufEmpty();

	int dropFrame(int cnt);

private:

	typedef struct myImg
	{
		imgHead head;
		vector<int> img;
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

