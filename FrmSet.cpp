#include "FrmSet.h"
#include <QSettings>
#include <QTextCodec>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileDialog>
#include "Global.h"
#include "MessageBox.h"

#pragma execution_character_set("utf-8")

FrmSet::FrmSet(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);


	setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint); //taskbar click min/restore

	setAcceptDrops(true);
	ui.listDir->setAcceptDrops(true);

	QSettings cfg(gPath.dataBase + "/config.ini", QSettings::IniFormat);
	cfg.setIniCodec(QTextCodec::codecForName("UTF8"));

	uint num = cfg.value("base/num", 3).toUInt();
	ui.spinNum->setValue(num);

	cfg.beginGroup("dirs");
	QStringList keys = cfg.allKeys();
	for (QString& key : keys) {
		QString s = cfg.value(key).toString();

		QStringList ls = s.split('|');
		listAddDir(ls[1], ls[0] == "1");
	}
	cfg.endGroup();

}

FrmSet::~FrmSet()
{
}

void FrmSet::on_btnAdd_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), ui.listDir->item(0)->text(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (!dir.isEmpty())
	{
		for (int i = 0; i < ui.listDir->count(); ++i)
		{
			if (dir == ui.listDir->item(i)->text()) 
			{
				CMessageBox::ShowMessageBox(QMessageBox::Warning, "错误", "该文件夹已经存在！",
					QMessageBox::Ok);
				return;
			}
		}

		listAddDir(dir);
	}
}

void FrmSet::on_btnDel_clicked()
{
	if (CMessageBox::ShowMessageBox(QMessageBox::Question, "删除消息", "确定要删除该文件夹？",
		QMessageBox::Ok | QMessageBox::Cancel)
		== QMessageBox::Ok)
	{
		
		ui.listDir->takeItem(ui.listDir->currentRow());

	}
}

void FrmSet::on_btnOK_clicked()
{
	QSettings cfg(gPath.dataBase + "/config.ini", QSettings::IniFormat);
	cfg.setIniCodec(QTextCodec::codecForName("UTF8"));

	cfg.setValue("base/num", ui.spinNum->value());

	cfg.beginGroup("dirs");

	//int n = 0;
	for (int i=0; i< ui.listDir->count();++i)
	{
		if (ui.listDir->item(i)->checkState() == Qt::Checked)
		{
			cfg.setValue(QString::number(i), "1|" + ui.listDir->item(i)->text());
		}
		else
		{
			cfg.setValue(QString::number(i), "0|" + ui.listDir->item(i)->text());
		}
		//n++;
	}

	cfg.endGroup();


	accept();
}

void FrmSet::on_btnCancel_clicked()
{
	reject();
}

void FrmSet::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
	{
		event->acceptProposedAction();
	}
}

void FrmSet::dragMoveEvent(QDragMoveEvent* event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
	{
		event->acceptProposedAction();
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

void FrmSet::dropEvent(QDropEvent* event)
{

	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;

	foreach(QUrl url, urls)
	{
		QString dir = url.toLocalFile();
		if (CFileHelper::isDirExist(dir))
		{
			listAddDir(dir);

		}
	}
}

void FrmSet::listAddDir(const QString& dir, bool checked)
{
	QListWidgetItem* item = new QListWidgetItem;
	item->setData(Qt::DisplayRole, "text");

	if (checked)
	    item->setData(Qt::CheckStateRole, Qt::Checked);
	else
		item->setData(Qt::CheckStateRole, Qt::Unchecked);

	item->setText(dir);
	ui.listDir->addItem(item);
}
