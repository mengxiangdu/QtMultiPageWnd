#pragma once

#include "qwidget.h"
#include "qgraphicseffect.h"

class QPushButton;
class QtMultiPageWnd;

class MDragableCtrl : public QWidget
{
	Q_OBJECT

public:
	MDragableCtrl(QWidget* parent = 0);
	QPushButton* addTitle(const QString& title);
	QPushButton* insertTitle(int where, const QString& title);
	QList<QPushButton*> buttons() const;
	bool empty() const;

signals:
	void clicked(int index);

private slots:
	void headButtonClicked();

private:
	int findHitButton(const QPoint& pos) const;
	QPixmap createWndSnapshot(QtMultiPageWnd* newWnd) const;
	void installDragEffect(const QPoint& pos);
	bool eventFilter(QObject* src, QEvent* event) override;
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

private:
	QList<QPushButton*> saveBtns;
	QPoint lastPt; /* 最近一次鼠标点击位置 */
};

/* 拖拽特效 */
class MDragEffect : public QGraphicsEffect
{
	Q_OBJECT

public:
	MDragEffect(QObject* parent = 0);
	void setPos(int x);
	void setIndex(int i);
	int index() const;

private:
	QRectF boundingRectFor(const QRectF& sourceRect) const override;
	void draw(QPainter* painter) override;

private:
	int mIndex;
	int xpos;
};

struct QtMimeType
{
	QtMimeType(QtMultiPageWnd* p = 0) : ptr(p) {}
	QByteArray toByteArray() const;
	static QtMimeType fromByteArray(const QByteArray& array);
	QtMultiPageWnd* ptr;
};

Q_DECLARE_METATYPE(QtMimeType)

QDataStream &operator<<(QDataStream &out, const QtMimeType &myObj);
QDataStream &operator>>(QDataStream &in, QtMimeType &myObj);


