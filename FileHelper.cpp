#include "FileHelper.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QTextStream>
#include <QCryptographicHash>
#include <QUrl>
#include <stdio.h>
#include <QDesktopWidget>
#include <QDebug>
#include <QTextCodec>
#include <QProcess>
#include <regex>
#include "StringHelper.h"
#ifdef Q_OS_WIN
#include <Windows.h>
#endif


#pragma execution_character_set("utf-8")


bool CFileHelper::matchFilePathWindows(const std::string& filePath) {
	//string str("^[a-zA-Z]:([\\\\/][^\\s\\\\/:*?<>\"|][^\\\\/:*?<>\"|]*)*([/\\\\])?$");
	// 仅匹配文件
	std::regex reg("^[a-zA-Z]:([\\\\/][^\\s\\\\/:*?<>\"|][^\\\\/:*?<>\"|]*)+$");
	try {
		if (std::regex_match(filePath, reg)) {
			//cout << "true" << endl;
			return true;
		}
		else {
			//cout << "false" << endl;
		}
	}
	catch (std::regex_error e) {
		//cout << e.what() << "\ncode: " << e.code();
	}

	return false;
}

QString CFileHelper::getUniqueName(const QString& fileName)
{
	QString filePath = fileName; //C:\Users\Public\Documents\uim.test.txt
	QFileInfo infoOriginal(fileName);

	//QString s1 = infoOriginal.baseName(); //uim
	//QString s2 = infoOriginal.completeBaseName(); //uim.test
	////QString s3 = infoOriginal.absoluteDir();
	//QString s4 = infoOriginal.absoluteFilePath(); //C:/Users/Public/Documents/uim.test.txt
	//QString s5 = infoOriginal.absolutePath(); //C:/Users/Public/Documents
	//QString s6 = infoOriginal.bundleName(); //空
	//QString s7 = infoOriginal.canonicalFilePath();//C:/Users/Public/Documents/uim.test.txt
	//QString s8 = infoOriginal.canonicalPath();//C:/Users/Public/Documents
	//QString s9 = infoOriginal.completeSuffix();//test.txt
	//QString s10 = infoOriginal.filePath();//C:/Users/Public/Documents/uim.test.txt
	//QString s11 = infoOriginal.fileName();//uim.test.txt
	//QString s12 = infoOriginal.path();//C:/Users/Public/Documents
	//QString s13 = infoOriginal.suffix();//txt

	uint i = 0;
	QFileInfo info(fileName);
	while (info.exists())
	{
		++i;
		filePath = infoOriginal.absolutePath();
		filePath = CFileHelper::ensurePathLastDelimiter(filePath);
		filePath += infoOriginal.completeBaseName() + "(" + QString::number(i) + ")." + infoOriginal.suffix();
		info.setFile(filePath);
	}
	return filePath;
}

bool CFileHelper::isFileExist(const QString& fullFileName)
{
	QFileInfo fileInfo(fullFileName);
	return fileInfo.exists();
}

QString CFileHelper::getFileExt(const QString& fullFileName)
{
	QFileInfo fileInfo(fullFileName);
	return std::move(fileInfo.suffix());
}

QString CFileHelper::getFileName(const QString& fullFileName)
{
	QFileInfo fileInfo(fullFileName);
	return std::move(fileInfo.fileName());
}

//上面的不支持跨平台
QString CFileHelper::getFileNameAlternative(const QString& fullFileName)
{
	QString name = fullFileName;
	if (name.right(1) == '\\' || name.right(1) == '/')
		name = name.left(name.length() - 1);
	int n = name.lastIndexOf('\\');
	if (n >= 0) {
		QString s = name.right(name.length() - n - 1);
		return std::move(s);
	}
	n = name.lastIndexOf('/');
	if (n >= 0) {
		QString s = name.right(name.length() - n - 1);
		return std::move(s);
	}
	return "";
}

//不带扩展名
QString CFileHelper::getFileBaseName(const QString& fullFileName)
{
	QFileInfo fileInfo(fullFileName);
	return std::move(fileInfo.baseName());
}

QString CFileHelper::getFilePath(const QString& fullFileName)
{
	QFileInfo fileInfo(fullFileName);
	return std::move(fileInfo.path());
}

bool CFileHelper::isDirExist(const QString& fullPath)
{
	QDir dir(fullPath);
	return dir.exists();
}

bool CFileHelper::isFile(const QString& fullPath)
{
	QFileInfo fileInfo(fullPath);
	return fileInfo.isFile();
}

bool CFileHelper::isDir(const QString& fullPath)
{
	QFileInfo fileInfo(fullPath);
	return fileInfo.isDir();
}

QString CFileHelper::readFile(const QString& fileName)
{
	QString displayString;
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return "";

	QTextStream in(&file);
	QString line = in.readLine();
	displayString.append(line);

	while (!line.isNull())
	{
		line = in.readLine();
		displayString.append(line);
	}

	return std::move(displayString);
}

bool CFileHelper::ensureDirExist(QString fullPath)
{
	QString path = fullPath;
#ifdef Q_OS_WIN
	path = path.replace("/", "\\");
#endif
	if (!isDirExist(path))
	{
		QDir d;
		return d.mkpath(path); //只创建一级子目录，即必须保证上级目录存在
	}

	return true;
}

QString CFileHelper::ensurePathLastDelimiter(const QString& path)
{
	QString dir = path;

#ifdef Q_OS_WIN
	if (dir.right(1) != "\\") dir += "\\";
#else
	if (dir.right(1) != "/") dir += "/";
#endif

	return std::move(dir);
}

QString CFileHelper::getFileRealMd5(const QString& filePath)
{
	QFile theFile(filePath);
	theFile.open(QIODevice::ReadOnly);
	QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
	theFile.close();
	//qDebug() << ba.toHex().constData();
	return std::move(QString(ba.toHex()));
}

QByteArray CFileHelper::getFileMd5(const QString& filePath)
{
	QFile localFile(filePath);

	if (!localFile.open(QFile::ReadOnly)) //E~J$%0A[3YPI6YP%7DEO4KZJG.jpg ，这种，不用readwrite会失败
	{
		//qDebug() << "file open error.";
		return nullptr;
	}

	QCryptographicHash ch(QCryptographicHash::Md5);

	quint64 totalBytes = localFile.size();
	if (totalBytes == 0) {
		QByteArray ba;
		return ba;
	}

	quint64 bytesHasRead = 0;
	quint64 bytesToRead = totalBytes;
// 	quint64 jumpSize = 1024 * 1024 * 10;
	quint64 jumpSize = totalBytes / 10;
	quint64 readSize = 1024 * 1024; //1M

	QByteArray buf;

	//新算法
	while (bytesHasRead < totalBytes)
	{
		localFile.seek(bytesHasRead);

	    buf.resize(0);
		buf = localFile.read(qMin(bytesToRead, readSize));//最后部分bytesToRead可能小于readSize的固定大小
		ch.addData(buf);

		if (jumpSize == 0) break;/*文件太小了*/

		bytesHasRead += jumpSize;
		bytesToRead -= jumpSize;
	}

// 	while (true)
// 	{
// 		if (bytesToRead > 0)
// 		{
// 			buf = localFile.read(qMin(bytesToRead, readSize));
// 			ch.addData(buf);
// 
// 			bytesHasRead += jumpSize;
// 			if (bytesToRead < jumpSize/*最后部分*/ || jumpSize == 0/*文件太小了*/) break;
// 			bytesToRead -= jumpSize;
// 
// 			localFile.seek(bytesHasRead);
// 
// 			buf.resize(0);
// 		}
// 		else
// 		{
// 			break;
// 		}
// 
// 		if (bytesHasRead >= totalBytes)
// 		{
// 			break;
// 		}
// 	}

	localFile.close();
	QByteArray md5 = ch.result();

	return md5; //字符串 tohex
}

QString CFileHelper::getFileMd5Str(const QString& filePath)
{
	return std::move(QString(getFileMd5(filePath).toHex()));
}

//#include <QImage>
//QByteArray getImageMd5(const QImage& image)
//{
//	QCryptographicHash ch(QCryptographicHash::Md5);
//
//	quint64 totalBytes = 0;
//	quint64 bytesWritten = 0;
//	quint64 bytesToWrite = 0;
//	quint64 jumpSize = 1024 * 1024 * 10;
//	quint64 readSize = 1024 * 1024;
//	QByteArray buf;
//
//	totalBytes = image.byteCount();
//	bytesToWrite = totalBytes;
//
//	while (1)
//	{
//		if (bytesToWrite > 0)
//		{
//			buf = localFile.read(qMin(bytesToWrite, readSize));
//			ch.addData(buf);
//
//			bytesWritten += jumpSize;
//			if (bytesToWrite < jumpSize) break;
//			bytesToWrite -= jumpSize;
//
//			localFile.seek(bytesWritten);
//
//			buf.resize(0);
//		}
//		else
//		{
//			break;
//		}
//
//		if (bytesWritten == totalBytes)
//		{
//			break;
//		}
//	}
//
//	QByteArray md5 = ch.result();
//
//	return md5; //字符串 tohex
//}

QString CFileHelper::getFileSizeDesc(qint64 llBytes)
{
	double bytes = static_cast<double>(llBytes);
	unsigned long cIter = 0;
	const char* pszUnits[] = {("B"), ("KB"), ("MB"), ("GB"), ("TB")};
	unsigned long cUnits = sizeof(pszUnits) / sizeof(pszUnits[0]);

	// move from bytes to KB, to MB, to GB and so on diving by 1024
	while (bytes >= 1024 && cIter < (cUnits - 1))
	{
		bytes /= 1024;
		cIter++;
	}
	char sc[256];

	snprintf(sc, sizeof(sc), ("%.2f %s"), bytes, pszUnits[cIter]);

	return std::move(QString::fromStdString(sc));
}

qint64 CFileHelper::getFileSize(const QString& filePath)
{
	QFileInfo info(filePath);
	return info.size();
}

quint64 CFileHelper::getDirSize(const QString& path)
{
	QDir dir(path);
	dir.setFilter(dir.filter() | QDir::Hidden | QDir::NoSymLinks);

	quint64 size = 0;

	//dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot)返回所有子目录，并进行过滤 
	foreach(QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		//若存在子目录，则递归调用dirFileSize()函数 
		size += getDirSize(path + QDir::separator() + subDir);
	}

	//dir.entryInfoList(QDir::Files)返回文件信息 
	foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Files))
	{
		//计算文件大小 
		size += fileInfo.size();
	}
	
	return size;
}

quint64 CFileHelper::getDirFilesCount(const QString& path)
{
	quint64 n = 0;
	QDir dir(path);
	dir.setFilter(dir.filter() | QDir::Hidden | QDir::NoSymLinks);

	//dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot)返回所有子目录，并进行过滤 
	foreach(QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		//若存在子目录，则递归调用dirFileSize()函数 
		n += getDirFilesCount(path + QDir::separator() + subDir);
	}

	foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Files))
	{
		n++;
	}

	return n;
}

void CFileHelper::openDir(const QString& dir)
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

void CFileHelper::openFile(const QString& file)
{
	bool is_open = QDesktopServices::openUrl(QUrl::fromLocalFile(file));
	//QString url = "file:///" + file;
	//bool is_open = QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode)); //有些机器返回failed 2
	if (!is_open)
	{
	}

	//#ifdef Q_OS_WIN
	//	
	//#else
	//	QString cmd = "xdg-open " + file;　//在linux下，可以通过system来xdg-open命令调用默认程序打开文件；
	//	system(cmd.toStdString().c_str());
	//#endif
}

void CFileHelper::recruseDirFiles(const QString& path, QStringList& outList, const QStringList& filterList)
{
	QDir dir(path);
	dir.setFilter(dir.filter() | QDir::Hidden|QDir::NoSymLinks);

	foreach(QFileInfo fi, dir.entryInfoList())
	{
		if (fi.isFile())
		{
			//qDebug() << fi.suffix();
			//qDebug() << "File :" << fi.fileName();
			//if (mfi.suffix())
			if (filterList.size() > 0)
			{
				if (filterList.contains(fi.suffix().toLower()))
					outList.append(fi.absoluteFilePath());
			}
			else
				outList.append(fi.absoluteFilePath());
		}
		else
		{
			if (fi.fileName() == "." || fi.fileName() == "..")continue;
			//qDebug() << "Entry Dir" << fi.absoluteFilePath();
			recruseDirFiles(fi.absoluteFilePath(), outList, filterList);
		}
	}
}

void CFileHelper::recruseDirFiles(const QString& path, QStringList& outList, bool includeSubDir /*= false*/)
{
	QDir dir(path);
	dir.setFilter(dir.filter() | QDir::Hidden | QDir::NoSymLinks);

	foreach(QFileInfo fi, dir.entryInfoList())
	{
		if (fi.isFile())
		{
			outList.append(fi.absoluteFilePath());
		}
		else if (includeSubDir)
		{
			if (fi.fileName() == "." || fi.fileName() == "..")continue;
			//qDebug() << "Entry Dir" << fi.absoluteFilePath();
			recruseDirFiles(fi.absoluteFilePath(), outList, includeSubDir);
		}
	}
}

void CFileHelper::locateFile(const QString& qsFilePath)
{
	QString path = qsFilePath;
	QString cmd;
#ifdef Q_OS_WIN
	path = path.replace("/", "\\");
	cmd = QString("/select,\"%1\"").arg(path);
	std::wstring s = cmd.toStdWString();
	QString dir = getUpDir(qsFilePath); //如果不指定路径参数，则第一次打开explorer可能定位不到
	ShellExecute(NULL, L"open", L"explorer.exe", s.c_str(), dir.toStdWString().c_str(), SW_SHOW);
#else
    path = path.replace("\\", "/");
	path = getUpDir(path);
	openDir(path);
	// cmd = QString("open -R %1").arg(path);
	//
	// QProcess process;
	// process.startDetached(cmd);
#endif
	
}

bool CFileHelper::deleteFile(const QString& filePath)
{
	QString path = filePath;
#ifdef Q_OS_WIN
	path = path.replace('/', '\\');
#endif
	try
	{
		QFile::setPermissions(path, QFile::WriteOwner);
	}
	catch (...)
	{
		
	}
	return QFile::remove(path);
}

void CFileHelper::readTxtFileToList(const QString& path, QStringList& outList, const char* codecName/*="UTF8"*/)
{
	QFile txtFile(path);
	if (txtFile.open(QFile::ReadOnly | QIODevice::Text))
	{
		QTextCodec* codec = QTextCodec::codecForName(codecName);
		QTextStream data(&txtFile);
		data.setCodec(codec);
		QString line;
		while (!data.atEnd())//逐行读取文本，并去除每行的回车
		{
			line = data.readLine();
			//line = codec->toUnicode(line.toLatin1());
			line.remove('\n');
			outList << line;
		}
	}
	txtFile.close();
}

void CFileHelper::writeTxtToFile(const QString& s, const QString& filePath)
{
	QFile file(filePath);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		stream << s << endl; 
	}
	file.close();
}

void CFileHelper::saveStringList(const QStringList& list, const QString filePath)
{
	CStringHelper::saveStringList(list, filePath);
}

void CFileHelper::loadStringList(const QString filePath, QStringList& list)
{
	CStringHelper::loadStringList(filePath, list);
}

QString CFileHelper::getUpDir(const QString& sPath)
{
	QString s = sPath;
	s = s.replace("\\", "/");
	s = s.left(s.lastIndexOf("/"));
	return s;
}

int CFileHelper::deleteDir(QString dirName)
{
	QDir dir(dirName);
	return dir.removeRecursively();

	//以下疑似有缓冲效果，就是删除后会过段时间生效
	// QDir directory(dirName);
	// if (!directory.exists())
	// {
	// 	return true;
	// }
	//
	// QString srcPath = QDir::toNativeSeparators(dirName);
	// if (!srcPath.endsWith(QDir::separator()))
	// 	srcPath += QDir::separator();
	//
	//
	// QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
	// bool error = false;
	// for (QStringList::size_type i = 0; i != fileNames.size(); ++i)
	// {
	// 	QString filePath = srcPath + fileNames.at(i);
	// 	QFileInfo fileInfo(filePath);
	// 	if (fileInfo.isFile() || fileInfo.isSymLink())
	// 	{
	// 		QFile::setPermissions(filePath, QFile::WriteOwner);
	// 		if (!QFile::remove(filePath))
	// 		{
	// 			error = true;
	// 		}
	// 	}
	// 	else if (fileInfo.isDir())
	// 	{
	// 		if (!deleteDir(fileInfo.absoluteFilePath()))
	// 		{
	// 			error = true;
	// 		}
	// 	}
	// }
	//
	// if (!directory.rmdir(QDir::toNativeSeparators(directory.path())))
	// {
	// 	error = true;
	// }
	// return !error;
}

//拷贝文件：
bool CFileHelper::copyFile(const QString& sourceFilePath, const QString& toFilePath, bool overwrite)
{
	QString toPath = toFilePath;
	toPath = toPath.replace("\\", "/");
	if (sourceFilePath == toPath) {
		return true;
	}
	if (!QFile::exists(sourceFilePath)) {
		return false;
	}
	QDir* createfile = new QDir;
	bool exist = createfile->exists(toPath);
	if (exist) {
		if (overwrite) {
			createfile->remove(toPath);
		}
	}//end if

	if (!QFile::copy(sourceFilePath, toPath))
	{
		return false;
	}
	return true;
}

//拷贝文件夹：
bool CFileHelper::copyDir(const QString& fromDir, const QString& toDir, bool overwrite, bool errorContine, const QVector<QString>* ignoreNames)
{
	QDir sourceDir(fromDir);
	QDir targetDir(toDir);
	if (!targetDir.exists()) {    /**< 如果目标目录不存在，则进行创建 */
		if (!targetDir.mkpath(toDir))
			return false;
		// if (!targetDir.mkdir(targetDir.absolutePath()))
		// 	return false;
	}

	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList) {
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isDir()) {    /**< 当为目录时，递归的进行copy */

			bool b = true;
			if (ignoreNames)
			{
				b = !ignoreNames->contains(fileInfo.fileName());
			}

			if (b) {
				if (!copyDir(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()), overwrite, errorContine, ignoreNames))
				{
					return false;
				}
					
			}
		}
		else {            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
			if (overwrite && targetDir.exists(fileInfo.fileName())) {
				targetDir.remove(fileInfo.fileName());
			}

			/// 进行文件copy
			bool b = true;
			if (ignoreNames)
			{
				b = !ignoreNames->contains(fileInfo.fileName());
			}

			if (b) {
				if (!QFile::copy(fileInfo.filePath(),
					targetDir.filePath(fileInfo.fileName()))) {
					QString s = fileInfo.fileName();
					qDebug() << "拷贝文件失败：" << s;

					if (errorContine)
						continue;
					else
					   return false;
				}
				else
				{
					// QString s = fileInfo.fileName();
					// qDebug() << "拷贝文件成功：" << s;
				}
			}
		}
	}
	return true;
}