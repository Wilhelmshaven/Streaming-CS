/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	��־ģ��

	��Ҫ����������
	��һ����¼������Ϣ���ı�
	�������¼�������ݵ��ı�

	ʹ�ã�

	void initLogModule(int queueCnt = N)����ʼ��ģ�飬�������������ڻ�ȡ�������ݣ���N-1��ʱ��㣩������Ҫ��¼������ν��

	void logError(int errCode = -1, string error = "")��д�������Ϣ

	void insertTimestamp(int index, double timestamp)��д��ʱ�������־��index����

	void shutdownModule()���ر�ģ��
*/
class logger
{
public:

	static logger *getInstance();

	static void initLogModule(int queueCnt = 9);

	static void logError(int errCode = -1, string error = "Unknown Error");

	static void insertTimestamp(int index, double timestamp);

	static void shutdownModule();

private:

	static vector<queue<double>> clockRecord;

	static int queueCnt;

	static fstream errFile;
	static fstream dataFile;

	//���ݵ���ʱ�佨����־��ǰ׺
	static void generatePath();

	//������־�ļ�
	static void createLogFile(fstream &file, string filePath);

	/*
		����˵��ɾ���ļ��к��ã�
	*/

	static void silentlyRemoveDir(const char * _dir);
	static void silentlyRemoveDir(const wchar_t * _dir);

	/*
		������ϢԤ�����������д�����Ϣ���ȶ�����
	*/

	/*
		����ģʽ���
	*/

	logger();

	static logger *instance;

	logger(const logger &);
	logger &operator=(const logger &);

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