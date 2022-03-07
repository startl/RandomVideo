#pragma once

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QUuid>
#include <QWidget>
#include <QFontMetrics>

class CStringHelper
{
public:
	//C语言版本，记得自己释放返回的内存
	char *str_replace(char *orig, char *rep, char *with);

	static QByteArray HexStringToByteArray(const QString& HexString);

	static QString ByteArrayToHexString(const QByteArray& data);

	static QString createUID();

	static QString tryToMultiLine(const QString& str, uint countsPerLine);
	static QString elidedText(const QFont& font, const QString& s, uint width, Qt::TextElideMode mode = Qt::ElideRight);
	static QString elidedText(QWidget* w, const QString& s);
	static uint textWidth(const QFont& font, const QString& s);
	/*inline string& ltrim(string &str) {
	string::iterator p = find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace)));
	str.erase(str.begin(), p);
	return str;
	}

	inline string& rtrim(string &str) {
	string::reverse_iterator p = find_if(str.rbegin(), str.rend(), not1(ptr_fun<int, int>(isspace)));
	str.erase(p.base(), str.end());
	return str;
	}

	inline string& trim(string &str) {
	ltrim(rtrim(str));
	return str;
	}*/

	static bool isDigital(const QString& s);

	static bool isMobile(const char* number);
	static bool isPhone(const char* number);
	static bool isMail(const char* number);

	static QStringList regexMatch(const QString& pattern, const QString& text);
	static int compareVersion(const QString& ver1, const QString& ver2); //-1：A小于B；0
	static bool isMobile(const QString& s);
	static bool isPhone(const QString& s);
	static bool isMail(const QString& s);

	static int getRandom(int min, int max);

	static void saveStringList(const QStringList& list, const QString filePath);
	static void loadStringList(const QString filePath, QStringList& list);

#ifndef Q_OS_WIN
	static int u2g(char* inbuf, int inlen, char* outbuf, int outlen);
	static int g2u(char* inbuf, size_t inlen, char* outbuf, size_t outlen);
#endif
};