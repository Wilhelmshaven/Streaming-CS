#include "CommonHeaders.h"

//�̳���Server��
class rtspServer
{
public:
	static rtspServer *getInstance()
	{
		return instance;
	}

	void test();

private:
	static rtspServer *instance;
	rtspServer(string file);

	//��ֹ���������Լ���ֵ
	rtspServer(const rtspServer &);
	rtspServer & operator = (const rtspServer &);

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

	// VARs
	string fileName;           //�����ļ���
	sockaddr_in srvAddr;       //��ַ�ṹ��ipv4/6��IP���˿ڣ�
	string displayAddr;        //�����Ĳ��ŵ�ַ��eg. http://www.rayion.com/desktop

	bool readFile();  // ��ȡ�����ļ�
};