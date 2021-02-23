#pragma once

#include <qlogging.h>
#include <iostream>
#include <fstream>

using namespace std;
#pragma execution_character_set("utf-8")

#define  LOG

#ifdef _DEBUG
#define  LOG_CON
#endif // DEBUG

class CLog
{
	static void openLogFile();
	static void writeLogMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg);
public:
	static void installLog();
	static void flush();
};
