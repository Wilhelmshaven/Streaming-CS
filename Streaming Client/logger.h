/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	��־ģ��

	��Ҫ����������
	��һ����¼������Ϣ���ı�
	�������¼�������ݵ��ı�

	ʹ�ã�
	void initLogModule()����ʼ��ģ��
	void logError(int errCode, string error)��д�������Ϣ
	void logDelayData(string data)��д���ӳ�����
	void shutdownModule()���ر�ģ��
*/
class logger
{
public:

	static logger *getInstance();

	static void initLogModule(int queueCnt = 8);

	static void logError(int errCode, string error);

	static void logData(string data);

	static void insertTimestamp(int pivot, double timestamp);

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