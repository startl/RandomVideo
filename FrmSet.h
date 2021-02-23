#pragma once

#include <QDialog>
#include "ui_FrmSet.h"

class FrmSet : public QDialog
{
	Q_OBJECT
private slots:
	void on_btnAdd_clicked();
	void on_btnDel_clicked();

	void on_btnOK_clicked();
	void on_btnCancel_clicked();
public:
	FrmSet(QWidget *parent = Q_NULLPTR);
	~FrmSet();

	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dropEvent(QDropEvent* event) override;

	void listAddDir(const QString& dir, bool checked = true);

private:
	Ui::FrmSet ui;
};
