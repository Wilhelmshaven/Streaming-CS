/*--Author������--*/

#pragma once
#include "CommonHeaders.h"

/*
	��־ģ��

	��Ҫ����������
	��һ����¼������Ϣ���ı�
	�������¼�������ݵ��ı�
*/

class logger
{
public:

	static logger *getInstance();

	void initLogModule();

	void logError(int errCode, string error);

	void logDelayData(int time);

	void shutdownModule();

private:

	fstream errFile;
	fstream delayDataFile;

	//���ݵ���ʱ�佨����־��ǰ׺
	void generatePath();

	//������־�ļ�
	void createLogFile(fstream &file, string filePath);

	/*
		����˵��ɾ���ļ��к��ã�
	*/

	void silentlyRemoveDir(const char * _dir);
	void silentlyRemoveDir(const wchar_t * _dir);

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