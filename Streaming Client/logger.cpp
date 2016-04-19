/*--Author：李宏杰--*/

#include "logger.h"

logger *logger::instance = new logger;

logger * logger::getInstance()
{
	return instance;
}

void logger::initLogModule()
{
	//生成文件名
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
		根据当天日期生成文件名

		格式：年月日+具体文件类型

		例如：20160515 ErrorLog.txt
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
	在磁盘上创建日志文件
	如果已经有了，就在文件后接着写，反正每天一个
*/
void logger::createLogFile(fstream &file, string filePath)
{
	file.open(filePath.c_str(), ios_base::out | ios_base::app);
}

void logger::silentlyRemoveDir(const char * _dir)
{
	// 如果结尾有 '\\' 则去掉结尾的 '\\'

	string dir(_dir);

	auto backSlashPos = dir.find_last_of('\\');
	if (backSlashPos == dir.size() - 1)
		dir.resize(dir.size() - 1);

	auto forwardSlashPos = dir.find_last_of('/');
	if (forwardSlashPos == dir.size() - 1)
		dir.resize(dir.size() - 1);

	dir += '\0'; // 字符串必须以两个 \0 结尾

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
	// 如果结尾有 '\\' 则去掉结尾的 '\\'

	wstring dir(_dir);

	auto backSlashPos = dir.find_last_of(L'\\');
	if (backSlashPos == dir.size() - 1)
		dir.resize(dir.size() - 1);

	auto forwardSlashPos = dir.find_last_of(L'/');
	if (forwardSlashPos == dir.size() - 1)
		dir.resize(dir.size() - 1);

	dir += L'\0'; // 字符串必须以两个 \0 结尾

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