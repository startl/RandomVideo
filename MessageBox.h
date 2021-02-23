#pragma once

#include <QMessageBox>

class CMessageBox : public QMessageBox
{
public:
	/*CMessageBox(Icon icon, const QString &title, const QString &text,
		StandardButtons buttons = NoButton, QWidget *parent = nullptr,
		Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);*/

	static int ShowMessageBox(Icon icon, const QString& title, const QString& text,
	                          StandardButtons buttons = NoButton, QWidget* parent = nullptr,
	                          Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	/*static int question(QWidget *parent, const QString &title,
		const QString& text,
		const QString& button0Text,
		const QString& button1Text = QString(),
		const QString& button2Text = QString(),
		int defaultButtonNumber = 0,
		int escapeButtonNumber = -1);

	static int information(Icon icon, const QString &title, const QString &text,
		StandardButtons buttons = NoButton, QWidget *parent = nullptr,
		Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	static int warning(Icon icon, const QString &title, const QString &text,
		StandardButtons buttons = NoButton, QWidget *parent = nullptr,
		Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	static int critical(Icon icon, const QString &title, const QString &text,
		StandardButtons buttons = NoButton, QWidget *parent = nullptr,
		Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);*/
};
