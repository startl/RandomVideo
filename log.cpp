#include "log.h"
#include <stdlib.h>
#include <mutex>
#include <QDateTime>
#include <QFile>
#include <QApplication>
#include <QStandardPaths>
#include "FileHelper.h"

#if defined(LOG_CON)
#if defined(Q_OS_WIN)
#include "qt_windows.h"
HANDLE m_outHandle;
#endif
#endif

#pragma execution_character_set("utf-8")

const uint LOG_WRITE_FREQUENCY = 1;

static std::mutex gLogMutex;



//支持中文路徑，並支持寫入unioce字符
class WOFSTREAM : public std::ofstream
{
public:
	WOFSTREAM()
	{
	}

	WOFSTREAM(const wchar_t* path)
	{
		//setlocale(LC_ALL, "");
		//size_t sz = -1;
		char szPath[1024] = {0};
		wcstombs(szPath, path, sizeof(szPath));

		cout << szPath << "\n";
		open(szPath, binary); //注意此处如不指定binary,ostream对象被默认视为对文本进行操作，会额外在行末添加字符：0x0D
		this->WriteBOM();
		//setlocale(LC_ALL, "C");
	}

	/*WOFSTREAM& operator <<(const wchar_t* text)
	{
	const char *pData = (const char *)text;
	const unsigned int length = wcslen(text) * sizeof(text[0]);
	write(pData, length);
	return *this;
	}
	WOFSTREAM& operator <<(unsigned short ch)
	{
	const char *pData = (char *)&ch;
	const unsigned int length = sizeof(ch);
	write(pData, length);
	return *this;
	}
	WOFSTREAM& operator<<(WOFSTREAM& (*pfunc)(WOFSTREAM&))
	{
	return ((*pfunc)(*this));
	}*/
protected:
	void WriteBOM()
	{
		const static wchar_t BOM = 0xfeff;
		write((const char *)&BOM, sizeof(BOM));
	}
};

WOFSTREAM& endl(WOFSTREAM& wf) //重载换行符
{
	wf << 0x000D << 0x000A;
	return wf;
};

static WOFSTREAM cFile;
QString CLog::logPath = "";

void CLog::openLogFile()
{
	/*file.setFileName(gUnify->App()->Path()->appDir + "log.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Append);*/
	//不要包含unify，update程序会引入多余文件
	cFile.open(CLog::logPath.toLocal8Bit().toStdString().c_str(), ios::app);
	//cFile.imbue(std::locale("chs"));
	cFile.imbue(locale(locale(), "", LC_CTYPE));
}

void CLog::writeLogMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	//static uint lineCounter = 0;
	//std::lock_guard<std::mutex> autoLock(gLogMutex);
	if (!cFile.is_open())
	{
		openLogFile();
	}

	QString level;
	switch (type)
	{
	case QtDebugMsg:
		level = QString("Debug:");
		break;

	case QtWarningMsg:
		level = QString("Warning:");
		break;

	case QtCriticalMsg:
		level = QString("Critical:");
		break;

	case QtFatalMsg:
		level = QString("Fatal:");
	}

	QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
	QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz ddd");
	//QString current_date = QString("(%1)").arg(current_date_time);
	QString message = QString("%1 %2 %3 %4").arg(current_date_time).arg(level).arg(context_info).arg(msg);

	//text_stream << message << "\r\n";
	cFile << message.toStdString().c_str() << endl;
	try
	{
		cFile.flush();
	}
	catch (...)
	{
		
	}

#if defined(LOG_CON)
#if defined(Q_OS_WIN)
	DWORD dwWrite = 0;
	wstring sw = message.toStdWString();
	WriteConsoleW(m_outHandle, sw.c_str(), wcslen(sw.c_str()), &dwWrite, nullptr);
	WriteConsoleA(m_outHandle, ("\n"), 1, &dwWrite, nullptr);
#endif
#endif

	//lineCounter++;
	//if (lineCounter >= LOG_WRITE_FREQUENCY) {
	//	lineCounter = 0;
	//	file.flush();
	//	//file.close();
	//	//openLogFile();
	//}
}

void CLog::installLog(const QString& logName, bool removeOld)
{
#ifndef Q_OS_WIN
	//在程序初始化之后，路径会增加程序名称，比如.local/share/,会变成.local/share/uim
	QString writableLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QString sysData = writableLocation + "/data/"; //data目录
	QString logBakDir = sysData + "logs/";
#else
	QString appDir = QApplication::applicationDirPath();
	appDir = appDir.replace('/', '\\');
	QString sysData = appDir + "\\data\\"; //data目录
	QString logBakDir = sysData + "logs\\";
#endif
	CLog::logPath = sysData + logName + ".txt";

#if defined(LOG)
	if (removeOld)
		CFileHelper::deleteFile(CLog::logPath);
	else
	{
		qint64 size = CFileHelper::getFileSize(CLog::logPath);
		if (size > 10 * 1024 * 1024)//10M
		{
			//QFile::remove(CLog::logPath+".old");
			CFileHelper::ensureDirExist(logBakDir);
			QDateTime dt = QDateTime::currentDateTime();
			QString logBakPath = logBakDir + dt.toString("yyyy-MM-dd hh.mm.ss") + ".txt";
			CFileHelper::copyFile(CLog::logPath, logBakPath, true);
			CFileHelper::deleteFile(CLog::logPath);
		}
	}
	qInstallMessageHandler(writeLogMessage);
#endif

#if defined(LOG_CON)
#if defined(Q_OS_WIN)
	AllocConsole();
	m_outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	/*while (m_outHandle == 0) {
	m_outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	}*/
	SetConsoleTitle(logName.toStdWString().c_str());
	//SetConsoleCP(936);

	freopen("CONOUT$", "w+t", stdout); // 申请写
	freopen("CONIN$", "r+t", stdin); // 申请读
#endif
#endif
}

void CLog::flush()
{
#if defined(LOG)
	cFile.flush();
	cFile.close();
#endif
}
