#pragma once

#include <qlogging.h>
#include <QString>
#include <iostream>
#include <fstream>

using namespace std;
#pragma execution_character_set("utf-8")

#define  LOG

#ifdef _DEBUG 
#define  LOG_CON
#endif // DEBUG

#ifdef TEST 
#define  LOG_CON
#endif // DEBUG

class CLog
{
	static void openLogFile();
	static void writeLogMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg);
public:
	static QString logPath;
	static void installLog(const QString& logName = "log", bool removeOld = true);
	static void flush();
};