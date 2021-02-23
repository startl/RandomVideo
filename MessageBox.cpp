#include "MessageBox.h"

#pragma execution_character_set("utf-8")

int CMessageBox::ShowMessageBox(Icon icon, const QString& title, const QString& text,
                                StandardButtons buttons /*= NoButton*/, QWidget* parent /*= nullptr*/,
                                Qt::WindowFlags flags /*= Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint*/)
{
	QMessageBox box(icon, title, text, buttons, parent, flags);
	//box.setStandardButtons(buttons);
	box.setButtonText(Ok, "确定");
	box.setButtonText(Cancel, "取消");
	box.setButtonText(Yes, "是");
	box.setButtonText(No, "否");
	box.setWindowFlags(Qt::WindowStaysOnTopHint);
	box.raise();
	box.activateWindow();
	return box.exec();
}
