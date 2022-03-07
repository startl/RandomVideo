#include "RandomVideo.h"
#include "FileHelper.h"
#include <QTime>
#include <QDebug>
#include <QProcess>
#include "Global.h"
#include <QScrollArea>
#include <QLabel>
#include <QDesktopServices>
#include <QUrl>
#include <QMenu>
#include <QTextCodec>
#include "MessageBox.h"
#include "TimeHelper.h"
#include "FrmSet.h"

#pragma execution_character_set("utf-8")

RandomVideo::RandomVideo(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setWindowState(Qt::WindowMaximized);

	ui.btnChange->setShortcut(tr("Space"));

	connect(ui.listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(onListWidgetItemDoubleClicked(QListWidgetItem*)));
	ui.listWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	connect(ui.listWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this,
		SLOT(onListWidgetContextMenu(const QPoint&)));

	//QSettings cfg(gPath.dataBase + "/" + CFileHelper::getFileBaseName(gPath.appName) + ".cfg", QSettings::IniFormat);
	reloadSettings();

	//this->installEventFilter(this);
	// for (int i = 0; i <= 180; i += 5) {
	// 	QString command = "\\%1.png\"";
	// 	command = command.arg(i, 2, 10, QChar('0')); //两位0x，不足补0
	// 	qDebug() << command;
	// }
	 freshFileList();
}


void RandomVideo::reloadSettings()
{
	QSettings cfg(gPath.dataBase + "/config.ini", QSettings::IniFormat);
	cfg.setIniCodec(QTextCodec::codecForName("UTF8"));

	mNumber = cfg.value("base/num", 3).toUInt();

	mDirs.clear();
	cfg.beginGroup("dirs");
	QStringList keys = cfg.allKeys();
	for (QString& key : keys) {
		QString s = cfg.value(key).toString();
		//mDirs.append(s);

		QStringList ls = s.split('|');
		if (ls[0] == "1")
			mDirs.append(ls[1]);
	}
	cfg.endGroup();
}

void RandomVideo::freshFileList()
{
	 mFileList.clear();

	 
	 //CFileHelper::readTxtFileToList(gPath.dataBase + "/DirList.cfg", dirs);

	 QStringList files, filters;
	 filters << "mp4" << "wmv" << "rm" << "rmvb" << "ram" << "avi" << "flv" << "ts" << "mpg"
	 << "mpeg" << "dat" << "m2v" << "vob" << "asf" << "mkv" <<"mov" <<"ogm"  << "webm" <<"3gp";
	 //filters << "wmv";
	for (auto& dir: mDirs)
	{
		CFileHelper::recruseDirFiles(dir, mFileList, filters);
	}
}

void RandomVideo::on_btnChange_clicked()
{
	if (mFileList.size() <= 0) return;

	if (!ui.btnChange->isEnabled()) return;

	ui.btnChange->setEnabled(false);
	ui.btnMultiThumb->setEnabled(false);
	ui.listWidget->clear();

	qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
	const uint ITEM_WIDTH = ui.listWidget->width();
	for (int i = 0; i < mNumber; i++)
	{
		int n = qrand() % mFileList.size() - 1;
		while (n==-1)
		{
			n = qrand() % mFileList.size() - 1;
		}
		//qDebug() << n;

		QString filePath = mFileList[n];

		tryBuildCache(filePath);

		filePath = filePath.replace("/", "\\");
		QString outDir = gPath.cache + CFileHelper::getFileName(filePath);
		outDir = outDir.replace("/", "\\");

		QStringList imgFileNames;
		CFileHelper::recruseDirFiles(outDir, imgFileNames);

		QWidget* pContainer = new QWidget;

		QLabel* pLblTitle = new QLabel(pContainer);
		pLblTitle->setObjectName(QStringLiteral("lblTilte"));
		pLblTitle->setStyleSheet("color:rgb(255,0,0)");
		pLblTitle->setGeometry(QRect(0, 0, ITEM_WIDTH, 25));
		//pLblTitle->setMargin(6);
		pLblTitle->setWordWrap(true);
		pLblTitle->setAlignment(Qt::AlignCenter);
		pLblTitle->setText(CFileHelper::getFileName(outDir));
		pLblTitle->setAttribute(Qt::WA_TransparentForMouseEvents, true);

		uint colCount = ITEM_WIDTH / 320;
		uint rowCount = imgFileNames.count() % colCount == 0 ? imgFileNames.count() / colCount : imgFileNames.count() / colCount + 1;

		uint needHeight = 0;
		for (int row = 0; row < rowCount; row++) {
			for (int col = 0; col < colCount; col++) {
				uint index = row * colCount + col;
				if (index >= imgFileNames.count()) break;

				QLabel* pLblImg = new QLabel(pContainer);
				pLblImg->setGeometry(QRect(320 * col, 180*row + 25, 320, 180));
				pLblImg->setFixedSize(320, 180);
				pLblImg->setPixmap(QPixmap::fromImage(QImage(imgFileNames[index])));
				pLblImg->setAttribute(Qt::WA_TransparentForMouseEvents, true);
			}
			needHeight += 180;
		}
		

		pContainer->setFixedSize(ITEM_WIDTH, needHeight + 20);
		QListWidgetItem* pItem = new QListWidgetItem(); //就是在新建项的时候，项的父控件不能是QListWidget，否则还是添加到QListWidget的最后
		pItem->setData(Qt::UserRole, filePath);
		pItem->setSizeHint(QSize(ITEM_WIDTH, needHeight + 20));

		ui.listWidget->addItem(pItem); 
		ui.listWidget->setItemWidget(pItem, pContainer);
	}

	ui.btnChange->setEnabled(true);
	ui.btnMultiThumb->setEnabled(true);
}

void RandomVideo::on_btnMultiThumb_clicked()
{
	if (ui.btnMultiThumb->property("stat").toInt() == 0)
	{
		ui.btnMultiThumb->setText("停建");
		ui.btnMultiThumb->setProperty("stat", 1);

		mBuildCanceld = false;

		ui.btnChange->setEnabled(false);
		for (int i = 0; i < mFileList.size(); i++)
			//for (int i = 0; i < 1; i++)
		{
			if (mBuildCanceld) break;

			QString filePath = mFileList[i];
			tryBuildCache(filePath);
		}

		ui.btnChange->setEnabled(true);

		ui.btnMultiThumb->setText("建缩图库");
		ui.btnMultiThumb->setProperty("stat", 0);
	}
	else
	{
		ui.btnMultiThumb->setText("建缩图库");

		mBuildCanceld = true;
		ui.btnMultiThumb->setProperty("stat", 0);
	}
	//ffmpeg -i “F:\迅雷下载\新建\ Bondage Breasts Upornia.com .mp4” -ss 00:00:10 -vframes 1 out.png
//ffmpeg -i test.flv -vf fps=1/600 thumb%04d.png
//ffprobe -i %s -show_entries format=duration -v quiet -of csv="p=0"
	
}

void RandomVideo::on_btnSet_clicked()
{
	FrmSet* ss = new FrmSet();
	ss->setWindowModality(Qt::ApplicationModal);
	if (ss->exec() == QDialog::Accepted)
	{
		reloadSettings();
		freshFileList();
	}

	ss->close();
	delete ss;
}

void RandomVideo::tryBuildCache(const QString& file)
{
	QString filePath = file;
	filePath = filePath.replace("/", "\\");
	QString outDir = gPath.cache + CFileHelper::getFileName(filePath);
	outDir = outDir.replace("/", "\\");

	if (!CFileHelper::isDirExist(outDir) || CFileHelper::getDirFilesCount(outDir) < 16)
	{
		CFileHelper::ensureDirExist(outDir);

		QString command = gPath.appDir + "ffprobe.exe -i \"%1\" -show_entries format=duration -v quiet -of csv=\"p=0\"";
		command = command.arg(filePath);
		QString length;
		QProcess pLen(0);
		connect(&pLen, &QProcess::readyReadStandardOutput, [&]()
			{
				length = QString::fromLocal8Bit(pLen.readAllStandardOutput());
				//qDebug() << length;
			});
		pLen.start(command);
		pLen.waitForFinished();

		QStringList times;
		times << "00:00:15";//first

		uint nLen = length.toDouble();
		uint step = nLen / 15;
		for (int i = 1; i <= 14; i++)
		{
			uint curSec = step * i;

			times << CTimeHelper::secToTimeStr(curSec);
		}

		times << CTimeHelper::secToTimeStr(nLen - 15); //last

		QProcess p(0);
		for (int i = 0; i < times.count(); i++)
		{
			QString outFile =outDir + "\\%1.png";
			outFile = outFile.arg(i, 2, 10, QChar('0'));

			//if (!CFileHelper::isFileExist(outFile)) 
			{
				command = gPath.appDir + "ffmpeg.exe -ss %1 -y -threads 32 " + "-i \"" + filePath +
					"\" -vframes 1 -vf \"scale=320:180:force_original_aspect_ratio=increase,crop=320:180\" \"" + outFile + "\"" ;
				command = command.arg(times[i]);

				//p.execute(command);
			    //p.waitForFinished();
				p.startDetached(command); //可以并行多个ffmpeg
			}
		}
			//qDebug() << QString::fromLocal8Bit(p.readAllStandardError());
			//int n = 1;
	}
}



void RandomVideo::onListWidgetItemDoubleClicked(QListWidgetItem* pItem)
{
	QString filePath = pItem->data(Qt::UserRole).value<QString>();

	QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void RandomVideo::onListWidgetContextMenu(const QPoint&)
{
	QMenu* menu = new QMenu(ui.listWidget); //border: 3px solid rgb(235,110,36);\ 
	menu->setCursor(Qt::PointingHandCursor);

	QListWidgetItem* pItem = ui.listWidget->currentItem();
	if (pItem)
	{
		QAction* actOpen = menu->addAction("打开");
		actOpen->setObjectName("open");
		connect(actOpen, SIGNAL(triggered(bool)), this, SLOT(onListWidgetMenuTriggered(bool)));

		QAction* sep = new QAction(menu);
		sep->setSeparator(true);
		menu->addAction(sep);

		QAction* actLocate = menu->addAction("定位文件");
		actLocate->setObjectName("locate");
		connect(actLocate, SIGNAL(triggered(bool)), this, SLOT(onListWidgetMenuTriggered(bool)));

		QAction* actDel = menu->addAction("删除文件");
		actDel->setObjectName("delete");
		connect(actDel, SIGNAL(triggered(bool)), this, SLOT(onListWidgetMenuTriggered(bool)));


		sep = new QAction(menu);
		sep->setSeparator(true);
		menu->addAction(sep);
	}

	QAction* actChange = menu->addAction("换一换");
	actChange->setObjectName("change");
	connect(actChange, SIGNAL(triggered(bool)), this, SLOT(onListWidgetMenuTriggered(bool)));

	menu->exec(QCursor::pos());
	delete menu;
}

void RandomVideo::onListWidgetMenuTriggered(bool b)
{
	QString name = sender()->objectName();

	if (name == "open")
	{
		QListWidgetItem* pItem = ui.listWidget->currentItem();
		onListWidgetItemDoubleClicked(pItem);
	}
	else if (name == "locate")
	{
		QListWidgetItem* pItem = ui.listWidget->currentItem();
		QString filePath = pItem->data(Qt::UserRole).value<QString>();
		CFileHelper::locateFile(filePath);
	}
	else if (name == "delete")
	{
		QListWidgetItem* pItem = ui.listWidget->currentItem();
		QString filePath = pItem->data(Qt::UserRole).value<QString>();

		if (CMessageBox::ShowMessageBox(QMessageBox::Question, "删除文件", QString("确定要删除文件 %1 ?").arg(CFileHelper::getFileName(filePath)),
			QMessageBox::Ok | QMessageBox::Cancel)
			== QMessageBox::Ok)
		{
			CFileHelper::deleteDir(gPath.cache + CFileHelper::getFileName(filePath));//删除缓存
			CFileHelper::deleteFile(filePath);

			QString upDir = CFileHelper::getUpDir(filePath);
			if (CFileHelper::getDirSize(upDir) <= 10 * 1024 * 1024)
			{
				qDebug() << "上级目录太小，删除：" << upDir;
				CFileHelper::deleteDir(upDir);
			}

			ui.listWidget->takeItem(ui.listWidget->currentRow());
		}
	}
	else if (name == "change")
	{
		on_btnChange_clicked();
	}
}

void RandomVideo::resizeEvent(QResizeEvent* size)
{
	if (mResizeCount < 2) 
	{
		mResizeCount++;
	}

	// if (mResizeCount==2)
	// {
	// 	on_btnChange_clicked();
	// }
	// for (int i=0; i < ui.listWidget->count(); i++)
 //    {
	// 	QListWidgetItem* pItem = ui.listWidget->item(i);
	// 	if (!pItem) continue;
	// 	pItem->setSizeHint(QSize(ui.listWidget->width() - 25, 200));
 //
	// 	QWidget* pContainer = ui.listWidget->itemWidget(pItem);
	// 	if (!pContainer) continue;
	// 	pContainer->setFixedSize(ui.listWidget->width() - 25, 200);
 //
	// 	QScrollArea* scrollArea = pContainer->findChild<QScrollArea*>("scrollArea");
	// 	if (!scrollArea) continue;
	// 	scrollArea->setFixedSize(ui.listWidget->width() - 25, 200);	
 //    }
}

bool RandomVideo::eventFilter(QObject* target, QEvent* event)
{
	// if (target == this && event->type() == QEvent::Show)
	// {
	// 	//if (mFirstShow)
	// 	{
	// 		uint x = ui.listWidget->width();
	// 		mFirstShow = false;
	// 	}
	// }
	
	return QMainWindow::eventFilter(target, event);
}
