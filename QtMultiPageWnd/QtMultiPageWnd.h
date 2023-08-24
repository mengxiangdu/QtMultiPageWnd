#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtMultiPageWnd.h"

class QtMultiPageWnd : public QWidget
{
    Q_OBJECT

public:
    QtMultiPageWnd(QWidget* defaultPage = 0, QWidget *parent = Q_NULLPTR);
	void addPage(QWidget* ipage);
	void insertPage(int where, QWidget* ipage);
	QWidget* takePage(int i);
	QPoint mapToFrame(const QPoint& pos) const;

private slots:
	void wiHeadClicked(int index);

private:
	void closeEvent(QCloseEvent* event) override;

private:
    Ui::QtMultiPageWndClass ui;
	bool isMainWnd;
};


