/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	ͼ�񻺴��ࣨͼ������ࣩ��������
	ͳһ���/���ڣ�vector<unsigned char>

	Ϊ�˼���ת������Ҫע��ʹ�÷���
	-->��ý������ģ���ڽ���ʱֱ��д�뵽�����У���������ģ��ֱ�Ӷ�ȡ����

	ʹ�ã�

	void pushBuffer(SOCKET index, imgHead head, shared_ptr<vector<BYTE>> img)�����뻺��

	bool popBuffer(SOCKET &index, imgHead &head, shared_ptr<vector<BYTE>> &img)����ȡ����ͷ�������Ƿ�ɹ�

	bool isBufEmpty()�����ػ����Ƿ�Ϊ��

	//int dropFrame(unsigned int cnt)��������ָ��������֡
*/
class imgBuffer
{
public:

	static imgBuffer* getInstance();

	void pushBuffer(SOCKET index, imgHead head, shared_ptr<vector<BYTE>> img);

	bool popBuffer(SOCKET &index, imgHead &head, shared_ptr<vector<BYTE>> &img);

	bool isBufEmpty();

	//int dropFrame(unsigned int cnt);

private:

	typedef struct myImg
	{
		SOCKET index;
		imgHead head;
		shared_ptr<vector<BYTE>> img;
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