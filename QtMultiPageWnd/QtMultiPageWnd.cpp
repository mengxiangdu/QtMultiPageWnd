#include "QtMultiPageWnd.h"
#include "MDragableCtrl.h"
#include "qpushbutton.h"
#include "qlabel.h"
#include "qdebug.h"

QtMultiPageWnd::QtMultiPageWnd(QWidget* defaultPage, QWidget *parent)
    : QWidget(parent), isMainWnd(!defaultPage)
{
    ui.setupUi(this);

	connect(ui.wiHead, &MDragableCtrl::clicked, this, &QtMultiPageWnd::wiHeadClicked);
	if (defaultPage) /* ��ӳ�ʼҳ */
	{
		addPage(defaultPage);
	}
	else /* û�г�ʼҳ��������Ĭ��ҳ*/
	{
		QLabel* lbl1 = new QLabel(u8"dasdjkasldjaskljasiof");
		lbl1->setWindowTitle(u8"��һҳ");
		addPage(lbl1);
		QLabel* lbl2 = new QLabel(u8"jyt45grdg56urshfsfgf");
		lbl2->setWindowTitle(u8"�ڶ�ҳ");
		addPage(lbl2);
		QLabel* lbl3 = new QLabel(u8"rewr234terfsdfdssdfsfsd");
		lbl3->setWindowTitle(u8"����ҳ");
		addPage(lbl3);
		QLabel* lbl4 = new QLabel(u8"fdergr54y45hrtj67764");
		lbl4->setWindowTitle(u8"����ҳ");
		addPage(lbl4);
	}
}

void QtMultiPageWnd::addPage(QWidget* ipage)
{
	QPushButton* btn = ui.wiHead->addTitle(ipage->windowTitle());
	ui.swContent->addWidget(ipage);
	emit btn->clicked();
}

void QtMultiPageWnd::insertPage(int where, QWidget* ipage)
{
	QPushButton* btn = ui.wiHead->insertTitle(where, ipage->windowTitle());
	ui.swContent->insertWidget(where, ipage);
	emit btn->clicked();
}

QWidget* QtMultiPageWnd::takePage(int i)
{
	QWidget* w = ui.swContent->widget(i);
	ui.swContent->removeWidget(w);
	w->setParent(0);
	return w;
}

QPoint QtMultiPageWnd::mapToFrame(const QPoint& pos) const
{
	QPushButton* btn = ui.wiHead->buttons().last();
	QPoint rawPos = btn->mapTo(this, pos);
	QPoint frameOffset(geometry().x() - frameGeometry().x(), geometry().y() - frameGeometry().y());
	return rawPos + frameOffset;
}

void QtMultiPageWnd::wiHeadClicked(int index)
{
	if (index >= 0)
	{
		ui.swContent->setCurrentIndex(index);
	}
}

void QtMultiPageWnd::closeEvent(QCloseEvent* event)
{
	/* ��������ջ�ϲ���delete */
	/* ֻ�ͷŷ������ڵ��ڴ� */
	if (!isMainWnd)
	{
		deleteLater();
	}
}




