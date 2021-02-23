#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RandomVideo.h"
#include <QSettings>

class RandomVideo : public QMainWindow
{
	Q_OBJECT
private slots:
	void on_btnChange_clicked();
	void on_btnMultiThumb_clicked();
	void on_btnSet_clicked();

	void onListWidgetItemDoubleClicked(QListWidgetItem* pItem);
	void onListWidgetContextMenu(const QPoint&);
	void onListWidgetMenuTriggered(bool b);
public:
	RandomVideo(QWidget *parent = Q_NULLPTR);

	void reloadSettings();

	void resizeEvent(QResizeEvent* size) override;
	bool eventFilter(QObject* target, QEvent* event);
private:
	QStringList mDirs;
	uint mNumber;
	bool mBuildCanceld = false;
	
	bool mFirstShow = true;
	uint mResizeCount = 0;

	Ui::RandomVideoClass ui;

	void freshFileList();

	QStringList mFileList;
	void tryBuildCache(const QString& file);
};
