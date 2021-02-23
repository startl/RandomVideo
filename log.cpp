#include "log.h"
#include <stdlib.h>
#include <QTime>
#include <QFileInfo>

#if defined(LOG_CON)
#if defined(Q_OS_WIN)
#include "qt_windows.h"
HANDLE m_outHandle;
#endif
#endif

#include "Global.h"
#include <QMutex>

const uint LOG_WRITE_FREQUENCY = 1;

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

void CLog::openLogFile()
{
	/*file.setFileName(gUnify->App()->Path()->appDir + "log.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Append);*/
	string s = (gPath.appDir + "log.txt").toStdString().c_str();
	cFile.open(s.c_str(), ios::app);
	//cFile.imbue(std::locale("chs"));
	cFile.imbue(locale(locale(), "", LC_CTYPE));
}

void CLog::writeLogMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	static QMutex mutex;
	//static uint lineCounter = 0;
	mutex.lock();

	//QTextStream text_stream;
	/*if (!file.isOpen()) {
		openLogFile();
		text_stream.setDevice(&file);
	}*/
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

	mutex.unlock();
}

void CLog::installLog()
{
#if defined(LOG)
	QFile::remove(gPath.appDir + "log.txt");
	qInstallMessageHandler(writeLogMessage);
#endif

#if defined(LOG_CON)
#if defined(Q_OS_WIN)
	AllocConsole();
	m_outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	/*while (m_outHandle == 0) {
	m_outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	}*/
	SetConsoleTitle(L"log");
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
