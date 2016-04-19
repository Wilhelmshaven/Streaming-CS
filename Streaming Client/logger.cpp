/*--Author������--*/

#include "logger.h"

logger *logger::instance = new logger;

logger * logger::getInstance()
{
	return instance;
}

void logger::initLogModule()
{
	//�����ļ���
	generatePath();
}

void logger::logError(int errCode, string error)
{
	errFile << errCode << "," << error << endl;
}

void logger::logDelayData(int time)
{
	delayDataFile << time << endl;
}

void logger::shutdownModule()
{
	errFile.close();

	delayDataFile.close();
}

void logger::generatePath()
{
	/*
		���ݵ������������ļ���

		��ʽ��������+�����ļ�����

		���磺20160515 ErrorLog.txt
	*/

	string prefix, errFilePath, delayDataPath;

	SYSTEMTIME date;
	GetSystemTime(&date);

	prefix.resize(9);
	sprintf_s((char *)prefix.data(), 9, "%04d%02d%02d", date.wYear, date.wMonth, date.wDay);
	prefix.erase(8);

	//silentlyRemoveDir("./logs");

	CreateDirectory("./logs", NULL);

	errFilePath = "./logs/" + prefix + " ErrorLog.txt";
	delayDataPath = "./logs/" + prefix + " DelayData.csv";

	createLogFile(errFile, errFilePath);
	createLogFile(delayDataFile, delayDataPath);
}

/*
	�ڴ����ϴ�����־�ļ�
	����Ѿ����ˣ������ļ������д������ÿ��һ��
*/
void logger::createLogFile(fstream &file, string filePath)
{
	file.open(filePath.c_str(), ios_base::out | ios_base::app);
}

void logger::silentlyRemoveDir(const char * _dir)
{
	// �����β�� '\\' ��ȥ����β�� '\\'

	string dir(_dir);

	auto backSlashPos = dir.find_last_of('\\');
	if (backSlashPos == dir.size() - 1)
		dir.resize(dir.size() - 1);

	auto forwardSlashPos = dir.find_last_of('/');
	if (forwardSlashPos == dir.size() - 1)
		dir.resize(dir.size() - 1);

	dir += '\0'; // �ַ������������� \0 ��β

	SHFILEOPSTRUCTA file_op = {
		NULL,
		FO_DELETE,
		dir.c_str(),
		"",
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT,
		false,
		0,
		"" };

	SHFileOperationA(&file_op);
}

void logger::silentlyRemoveDir(const wchar_t * _dir)
{
	// �����β�� '\\' ��ȥ����β�� '\\'

	wstring dir(_dir);

	auto backSlashPos = dir.find_last_of(L'\\');
	if (backSlashPos == dir.size() - 1)
		dir.resize(dir.size() - 1);

	auto forwardSlashPos = dir.find_last_of(L'/');
	if (forwardSlashPos == dir.size() - 1)
		dir.resize(dir.size() - 1);

	dir += L'\0'; // �ַ������������� \0 ��β

	SHFILEOPSTRUCTW file_op = {
		NULL,
		FO_DELETE,
		dir.c_str(),
		L"",
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT,
		false,
		0,
		L"" };

	SHFileOperationW(&file_op);
}

logger::logger()
{

}