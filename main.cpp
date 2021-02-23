#include "RandomVideo.h"
#include <QtWidgets/QApplication>
#include "log.h"
#include <QDir>
#include <QTextCodec>
#include "Global.h"

#pragma execution_character_set("utf-8")

int main(int argc, char *argv[])
{
	gPath.init(QString::fromLocal8Bit(argv[0]));
	
	CLog::installLog();
	
	QApplication a(argc, argv);
	RandomVideo w;
	w.show();
	return a.exec();
}
