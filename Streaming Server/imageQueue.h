/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

//OpenCVͷ
#include "opencv2\highgui\highgui.hpp"

using namespace cv;

/*
	ͼ�񻺴�ģ��
	ͳһ���/���ڣ�vector<int>��Mat
	�ͻ�����ɵ���vector<int> -> Mat�Ĺ���
	�������ɵ���Mat -> vector<int>�Ĺ���
*/

/*
	ͼ��ͷ���������ݴ���ϵͳ�����׼ȷ����

	������

	int height/rows��ͼ��߶�/����

	int width/cols��ͼ����/����

	int channels��ͼ��ͨ����

	int imgType��ͼ�����ͣ���RGB��BGR��YUV֮�ࣨ����ЩֵĿǰδʵ�֣�

	//int matrixType���������ͣ����������Ƿ���ϡ�����Ŀǰ�����������ݣ���ʱע��δʵ�֡�
*/
typedef struct imgHead
{
	union
	{
		int height;
		int rows;
	};

	union
	{
		int width;
		int cols;
	};

	int channels;

	//ͼ�����ͣ���RGB��BGR��YUV֮�ࣨ����ЩֵĿǰδʵ�֣�
	int imgType;

	//int matrixType;
};

/*
	ͼ�񻺴��ࣨͼ������ࣩ��������

	Ϊ�˼���ת������Ҫע��ʹ�÷���
	-->��ý������ģ���ڽ���ʱֱ��д�뵽�����У���������ģ��ֱ�Ӷ�ȡ����

	ʹ�ã�

	static imgBuffer* getInstance()����ȡ����

	void pushBuffer(imgHead head, Mat img)������vector<int>���͵������У�ͬʱ�������ת����

	bool popBuffer(imgHead &head, vector<int> &img)����ȡ����ͷ������imgHeadָ����vector<int>ָ�빩д�룬�����Ƿ�ɹ�

	bool isBufEmpty()�����ػ����Ƿ�Ϊ��
*/
class imgBuffer
{
public:

	static imgBuffer* getInstance();

	void pushBuffer(Mat img);

	bool popBuffer(imgHead &head, vector<int> &img);

	bool isBufEmpty();

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