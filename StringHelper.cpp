#include "StringHelper.h"
#include <regex>
#include <QTextCodec>
#include <QTime>
#include <math.h>
#include <QFile>

char *CStringHelper::str_replace(char *orig, char *rep, char *with) {
	char *result; // the return string
	char *ins;    // the next insert point
	char *tmp;    // varies
	int len_rep;  // length of rep (the string to remove)
	int len_with; // length of with (the string to replace rep with)
	int len_front; // distance between rep and end of last rep
	int count;    // number of replacements

	// sanity checks and initialization
	if (!orig || !rep)
		return NULL;
	len_rep = strlen(rep);
	if (len_rep == 0)
		return NULL; // empty rep causes infinite loop during count
	if (!with)
		return NULL;
	if (strstr(orig, rep) == NULL)//not found
	{
		result = (char*)malloc(strlen(orig) + 1);
		strcpy(result, orig);
		return result;
	}

	len_with = strlen(with);

	// count the number of replacements needed
	ins = orig;
	for (count = 0; tmp = strstr(ins, rep); ++count) {
		ins = tmp + len_rep;
	}

	tmp = result = (char*)malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if (!result)
		return NULL;

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);
	return result;
}


QByteArray CStringHelper::HexStringToByteArray(const QString& HexString)
{
	QString sHex = HexString;
	bool ok;
	QByteArray ret;
	sHex = sHex.trimmed();
	sHex = sHex.simplified();
	QStringList sl = sHex.split(" ");

	foreach(QString s, sl)
	{
		if (!s.isEmpty())
		{
			char c = s.toInt(&ok, 16) & 0xFF;
			if (ok)
			{
				ret.append(c);
			}
			else
			{
				qDebug() << "非法的16进制字符：" << s;
				//QMessageBox::warning(0, tr("错误："), QString("非法的16进制字符: \"%1\"").arg(s));
			}
		}
	}
	//qDebug() << ret;
	return std::move(ret);
}

QString CStringHelper::ByteArrayToHexString(const QByteArray& data)
{
	QString ret(data.toHex().toUpper());
	int len = ret.length() / 2;
	//qDebug() << len;
	for (int i = 1; i < len; i++)
	{
		//qDebug() << i;
		ret.insert(2 * i + i - 1, " ");
	}

	return std::move(ret);
}

QString CStringHelper::createUID()
{
	QUuid id = QUuid::createUuid();
	QString strId = id.toString();

	strId.remove("{").remove("}").remove("-"); // 一般习惯去掉左右花括号和连字符
	return std::move(strId);
}

QString CStringHelper::tryToMultiLine(const QString& str, uint countsPerLine)
{
	QString sMulti = "";
	QString s = str;

	if (s.length() > countsPerLine) {
		int n = ceil(float(s.length()) / float(countsPerLine)); //行数
		for (int i = 0; i < n; ++i) {
			sMulti += s.mid(i*countsPerLine, countsPerLine) + "\n";
		}
		sMulti = sMulti.left(sMulti.length() - 1);//去掉末尾\n
		return sMulti;
	}
	else {
		return str;
	}

// 	QString sMulti;
// 	QString s = str;
// 	while (s.length() > countsPerLine)
// 	{
// 		sMulti += s.left(countsPerLine) + "\n";
// 		s = s.right(s.length() - countsPerLine);
// 	}
// 	sMulti += s;
// 	return std::move(sMulti);
}

QString CStringHelper::elidedText(const QFont& font, const QString& s, uint width, Qt::TextElideMode mode)
{
	if (s.isEmpty())
	{
		return s;
	}

	QString str = s;
	QFontMetrics fontMetrics(font);
	int fontSize = fontMetrics.width(s);

	if (mode == Qt::ElideRight)
	{
		if (fontSize > width - fontMetrics.width("..."))
		{
			str = fontMetrics.elidedText(s, mode, width - fontMetrics.width("..."));
		}
	}
	else if (mode == Qt::ElideMiddle)
	{
		str = fontMetrics.elidedText(s, mode, width);
// 		QString out1, out2;
// 		bool bOutLimit = false;
// 		while (fontMetrics.width(s) > width)
// 		{
// 			bOutLimit = true;
// 			int n = str.length() / 2;
// 			out1 = str.left(n - 1);
// 			out2 = str.right(n);
// 			str = out1 + out2;
// 		}
// 		if (bOutLimit)
// 			str = out1 + "..." + out2;
	}

	return std::move(str);
}

QString CStringHelper::elidedText(QWidget* w, const QString& s)
{
	QString str = s;
	QFontMetrics fontMetrics(w->font());
	int fontSize = fontMetrics.width(s);
	int width = w->width();
	//qDebug() << width;
	if (fontSize > w->width() - fontMetrics.width("...") * 2)
	{
		str = fontMetrics.elidedText(s, Qt::ElideRight, w->width() - fontMetrics.width("...") * 2);
	}

	return std::move(str);
}

uint CStringHelper::textWidth(const QFont& font, const QString& s)
{
	QString str = s;
	QFontMetrics fontMetrics(font);
	return std::move(fontMetrics.width(s));
}

bool CStringHelper::isDigital(const QString& s)
{
	return s.contains(QRegExp("^\\d+$"));
}

bool CStringHelper::isMobile(const char* number)
{
	std::regex e("^1(3\\d|47|5([0-3]|[5-9])|8(0|2|[5-9]))\\d{8}$");

	return regex_match(number, e);
}

bool CStringHelper::isMobile(const QString& s)
{
	std::regex e("^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\\d{8}$"); 
	//std::regex e("^1(3\\d|47|5([0-3]|[5-9])|8(0|2|[5-9]))\\d{8}$");

	return regex_match(s.toStdString().c_str(), e);
}

bool CStringHelper::isPhone(const char* number)
{
	std::regex e("^(0[0-9]{2,3}\\-)?([2-9][0-9]{6,7})+(\\-[0-9]{1,4})?$");

	return regex_match(number, e);
}

bool CStringHelper::isPhone(const QString& s)
{
	std::regex e("^(0[0-9]{2,3}\\-)?([2-9][0-9]{6,7})+(\\-[0-9]{1,4})?$");

	return regex_match(s.toStdString().c_str(), e);
}

bool CStringHelper::isMail(const char* number)
{
	std::regex e("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");

	return regex_match(number, e);
}

bool CStringHelper::isMail(const QString& s)
{
	std::regex e("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");

	return regex_match(s.toStdString().c_str(), e);
}

int CStringHelper::getRandom(int min, int max)
{
	qsrand(QTime(0, 0, 0).msecsTo(QTime::currentTime()));

	int num = qrand() % (max - min);
	//qDebug() << num + min;

	return num;
}

void CStringHelper::saveStringList(const QStringList& list, const QString filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		return;
	}
	QDataStream ds(&file);
	ds << list;
}

void CStringHelper::loadStringList(const QString filePath, QStringList& list)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}
	QDataStream ds(&file);
	ds >> list;
}

QStringList CStringHelper::regexMatch(const QString& pattern, const QString& text)
{
	QStringList list;

	std::regex _rx(pattern.toStdString().c_str(), std::regex::icase);

	std::match_results<std::string::const_iterator> result;
	std::string sd = text.toStdString().c_str();
	/*if (std::regex_match(sd, result, _rx)) {
		std::string temp = result[0];
		cout << temp << endl;
		int n = 1;
	}*/
	//迭代器声明
	std::string::const_iterator iter = sd.begin();
	std::string::const_iterator iterEnd = sd.end();
	//正则查找
	while (std::regex_search(iter, iterEnd, result, _rx))
	{
		std::string temp = result[0];
		if (temp.empty()) break;

		list.append(QString::fromStdString(temp));
		//cout << temp << endl;
		iter = result[0].second; //更新搜索起始位置
	}

	return std::move(list);
}

int CStringHelper::compareVersion(const QString& ver1, const QString& ver2) //-1：A小于B；0: A等于B；1：A大于B
{
	if (ver1 == ver2) return 0;
	if (ver1.isEmpty()) return 0;
	if (ver2.isEmpty()) return 0;

	for (size_t i = 0; i < ver1.length(); i++)
	{
		// 如果指定版本号中的当前字符在0到9范围以外，并且不是分割点，就认为是无效的
		if ((ver1.at(i) < '0' || ver1.at(i) > '9') && ver1.at(i) != '.')
			return 0;
	}
	for (size_t i = 0; i < ver2.length(); i++)
	{
		// 如果指定版本号中的当前字符在0到9范围以外，并且不是分割点，就认为是无效的
		if ((ver2.at(i) < '0' || ver2.at(i) > '9') && ver2.at(i) != '.')
			return 0;
	}

	QStringList listA = ver1.split(".");
	QStringList listB = ver2.split(".");

	if (listA.count() >= listB.count())
	{
		for (int i = 0; i < listB.count(); i++)
		{
			if (listA[i].toInt() > listB[i].toInt())
				return 1;
			if (listA[i].toInt() < listB[i].toInt())
				return -1;
		}
	}
	else if (listA.count() < listB.count())
	{
		for (int i = 0; i < listA.count(); i++)
		{
			if (listA[i].toInt() > listB[i].toInt())
				return 1;
			if (listA[i].toInt() < listB[i].toInt())
				return -1;
		}
	}

	return 0;
}

#ifndef Q_OS_WIN
#include "/usr/include/iconv.h"
int code_convert(char* from_charset, char* to_charset, char* inbuf, int inlen, char* outbuf, int outlen)
{
	iconv_t cd;
	int rc;
	char** pin = &inbuf;
	char** pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0) return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, (size_t*)&inlen, pout, (size_t*)&outlen) == -1) return -1;
	iconv_close(cd);
	return 0;
}
//UNICODE码转为GB2312码
int CStringHelper::u2g(char* inbuf, int inlen, char* outbuf, int outlen)
{
	return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}
//GB2312码转为UNICODE码
int CStringHelper::g2u(char* inbuf, size_t inlen, char* outbuf, size_t outlen)
{
	return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}
#endif // Q_OS_LINUX


