﻿#pragma once

#include <QString>

class CFileHelper
{
public:
	static bool matchFilePathWindows(const std::string& filePath);
	static QString getUniqueName(const QString& fileName);
	static QString getFileExt(const QString& fullFileName);
	static QString getFileName(const QString& fullFileName);
	static QString getFileNameAlternative(const QString& fullFileName);
	static QString getFileBaseName(const QString& fullFileName);
	static QString getFilePath(const QString& fullFileName);

	static bool isFileExist(const QString& fullFileName);
	static bool isDirExist(const QString& fullPath);
	static bool isFile(const QString& fullPath);
	static bool isDir(const QString& fullPath);
	
	static QString readFile(const QString& fileName);
	static void readTxtFileToList(const QString& path, QStringList& outList, const char* codecName ="UTF8");
	static void writeTxtToFile(const QString& s, const QString& filePath);

	static void saveStringList(const QStringList& list, const QString filePath);
	static void loadStringList(const QString filePath, QStringList& list);

	static QString getUpDir(const QString& sPath);
	static bool ensureDirExist(QString fullPath);
	static QString ensurePathLastDelimiter(const QString& path);

	//文件全部计算，不跳过部分
	static QString getFileRealMd5(const QString& filePath);

	//支持超大文件，文件跳着计算，速度快
	static QByteArray getFileMd5(const QString& filePath);
	static QString getFileMd5Str(const QString& filePath);

	static QString getFileSizeDesc(qint64 llBytes);
	static qint64 getFileSize(const QString& filePath);
	static quint64 getDirSize(const QString& path);
	static quint64 getDirFilesCount(const QString& path);

	static void recruseDirFiles(const QString& dir, QStringList& outList, const QStringList& filterList);
	static void recruseDirFiles(const QString& dir, QStringList& outList, bool includeSubDir = false);

	static void openDir(const QString& dir);
	static void openFile(const QString& file);
	static void locateFile(const QString& qsFilePath);
	static bool deleteFile(const QString& filePath);
	static int deleteDir(QString dirName);
	static bool copyFile(const QString& sourceFilePath, const QString& toFilePath, bool overwrite);
	static bool copyDir(const QString& fromDir, const QString& toDir, bool overwrite, bool errorContine = false,  const QVector<QString>* ignoreNames =nullptr);
};
