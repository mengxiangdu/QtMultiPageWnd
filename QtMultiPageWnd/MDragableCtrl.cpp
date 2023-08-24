#include "MDragableCtrl.h"
#include "QtMultiPageWnd.h"
#include "qpushbutton.h"
#include "qboxlayout.h"
#include "qregularexpression.h"
#include "qevent.h"
#include "qmimedata.h"
#include "qcursor.h"
#include "qdrag.h"
#include "qpainter.h"
#include "qdebug.h"

MDragableCtrl::MDragableCtrl(QWidget* parent) : 
	QWidget(parent), lastPt(-1, -1)
{
	setAcceptDrops(true);
	QHBoxLayout* hlLayout = new QHBoxLayout(this);
	hlLayout->setMargin(0);
	hlLayout->addItem(new QSpacerItem(24, 24, QSizePolicy::Expanding));
	setLayout(hlLayout);
}

QPushButton* MDragableCtrl::addTitle(const QString& title)
{
	QPushButton* pbLast = new QPushButton(title, this);
	pbLast->setObjectName(QString(u8"pbPage%1").arg(1 + saveBtns.size()));
	pbLast->installEventFilter(this);
	connect(pbLast, &QPushButton::clicked, this, &MDragableCtrl::headButtonClicked);
	dynamic_cast<QHBoxLayout*>(layout())->insertWidget(saveBtns.size(), pbLast);
	saveBtns.push_back(pbLast);
	return pbLast;
}

QPushButton* MDragableCtrl::insertTitle(int where, const QString& title)
{
	QPushButton* pbLast = new QPushButton(title, this);
	pbLast->setObjectName(QString(u8"pbPage%1").arg(1 + saveBtns.size()));
	pbLast->installEventFilter(this);
	connect(pbLast, &QPushButton::clicked, this, &MDragableCtrl::headButtonClicked);
	dynamic_cast<QHBoxLayout*>(layout())->insertWidget(where, pbLast);
	saveBtns.insert(where, pbLast);
	return pbLast;
}

void MDragableCtrl::headButtonClicked()
{
	QPushButton* btn = dynamic_cast<QPushButton*>(sender());
	int index = saveBtns.indexOf(btn);
	emit clicked(index);
}

QList<QPushButton*> MDragableCtrl::buttons() const
{
	return saveBtns;
}

bool MDragableCtrl::empty() const
{
	return saveBtns.empty();
}

bool MDragableCtrl::eventFilter(QObject* src, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		lastPt = dynamic_cast<QMouseEvent*>(event)->pos();
	}
	else if (event->type() == QEvent::MouseMove)
	{
		QMouseEvent* tempEvent = dynamic_cast<QMouseEvent*>(event);
		QWidget* thisWidget = dynamic_cast<QWidget*>(src);
		if ((lastPt - tempEvent->pos()).manhattanLength() < QApplication::startDragDistance() && 
			lastPt == QPoint(-1, -1))
		{
			return false;
		}
		int which = findHitButton(thisWidget->mapToParent(tempEvent->pos()));
		if (which >= 0)
		{
			dynamic_cast<QHBoxLayout*>(layout())->removeWidget(saveBtns[which]);
			qDebug() << "ttt" << saveBtns[which];
			delete saveBtns[which];
			saveBtns.removeOne(saveBtns[which]);

			QtMultiPageWnd* myself = dynamic_cast<QtMultiPageWnd*>(topLevelWidget());
			QWidget* movePage = myself->takePage(which);
			QtMultiPageWnd* newWnd = new QtMultiPageWnd(movePage);
			newWnd->move(2048, 2048); /* 不显示无法获取正确的frameGeometry()值 */
			newWnd->show();
			/* 空了先隐藏自己 */
			/* 拖拽结束再关闭自己，见下方代码 */
			if (empty())
			{
				myself->hide();
			}

			QDrag drag(this);
			QMimeData *mimeData = new QMimeData;
			mimeData->setData(u8R"(application/x-qt-windows-mime;value="QtMimeType")", QtMimeType(newWnd).toByteArray());
			drag.setMimeData(mimeData);
			drag.setPixmap(createWndSnapshot(newWnd));
			drag.setHotSpot(newWnd->mapToFrame(tempEvent->pos()));

			Qt::DropAction dropAction = drag.exec();
			if (dropAction == Qt::IgnoreAction)
			{
				newWnd->move(QCursor::pos() - drag.hotSpot());
			}
			/* 如果本窗口已经空了，则关闭自己 */
			/* 注意不能直接delete，万一本窗口是主窗口内存在栈上删除会报错 */
			if (empty())
			{
				myself->close();
			}
			return true;
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease)
	{
		lastPt = QPoint(-1, -1);
	}
	return false;
}

QPixmap MDragableCtrl::createWndSnapshot(QtMultiPageWnd* newWnd) const
{
	QRect rawRect = newWnd->geometry();
	QRect frameRect = newWnd->frameGeometry();
	QPixmap pixmap(frameRect.size());
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setOpacity(0.75);
	painter.fillRect(pixmap.rect(), qRgba(255, 255, 255, 195));
	painter.setViewport(rawRect.x() - frameRect.x(), rawRect.y() - frameRect.y(), rawRect.width(), rawRect.height());
	newWnd->render(&painter);
	return pixmap;
}

int MDragableCtrl::findHitButton(const QPoint& pos) const
{
	int which = -1;
	for (int i = 0; i < saveBtns.size(); i++)
	{
		if (saveBtns[i]->geometry().contains(pos))
		{
			which = i;
			break;
		}
	}
	return which;
}

void MDragableCtrl::dragEnterEvent(QDragEnterEvent *event)
{
	installDragEffect(event->pos());
	event->accept();
}

void MDragableCtrl::dragMoveEvent(QDragMoveEvent *event)
{
	installDragEffect(event->pos());
}

void MDragableCtrl::dragLeaveEvent(QDragLeaveEvent *event)
{
	setGraphicsEffect(0);
}

void MDragableCtrl::dropEvent(QDropEvent *event)
{
	const QMimeData* mime = event->mimeData();
	QtMimeType data = QtMimeType::fromByteArray(mime->data(u8R"(application/x-qt-windows-mime;value="QtMimeType")"));
	QWidget* onePage = data.ptr->takePage(0);
	delete data.ptr; /* 这里可以delete因一定不是主窗口 */

	int insertWhere = dynamic_cast<MDragEffect*>(graphicsEffect())->index();
	setGraphicsEffect(0);

	dynamic_cast<QtMultiPageWnd*>(topLevelWidget())->insertPage(insertWhere, onePage);
	event->acceptProposedAction();
}

void MDragableCtrl::installDragEffect(const QPoint& pos)
{
	int where = saveBtns.count();
	for (int i = 0; i < saveBtns.count(); i++)
	{
		QRect region = saveBtns[i]->geometry();
		int cx = region.center().x();
		if (pos.x() < cx)
		{
			where = i;
			break;
		}
	}
	int xpos = 0;
	if (where > 0)
	{
		int space = dynamic_cast<QHBoxLayout*>(layout())->spacing();
		xpos = saveBtns[where - 1]->geometry().right() + space / 2;
	}
	MDragEffect* effect = new MDragEffect(this);
	effect->setIndex(where);
	effect->setPos(xpos);
	setGraphicsEffect(effect);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

MDragEffect::MDragEffect(QObject* parent) : 
	QGraphicsEffect(parent)
{
}

void MDragEffect::setPos(int x)
{
	xpos = x;
	update();
}

void MDragEffect::setIndex(int i)
{
	mIndex = i;
}

int MDragEffect::index() const
{
	return mIndex;
}

QRectF MDragEffect::boundingRectFor(const QRectF& sourceRect) const
{
	return sourceRect.adjusted(-4, 0, 4, 0);
}

void MDragEffect::draw(QPainter* painter)
{
	const QPoint triangle[] = { { -3, 0 }, { 3, 0 }, { 0, 3 } };
	QPoint offset;
	QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset);

	painter->save();
	painter->setWorldTransform(QTransform());
	painter->translate(offset);
	painter->drawPixmap(0, 0, pixmap);
	painter->setBrush(Qt::darkGray);
	painter->translate(xpos + 4, 0); /* +4是相当于sourceRect的0点 */
	painter->drawPolygon(triangle, 3);
	painter->restore();
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

QByteArray QtMimeType::toByteArray() const
{
	QByteArray stream;
	QDataStream ds(&stream, QIODevice::WriteOnly);
	ds << *this;
	return stream;
}

QtMimeType QtMimeType::fromByteArray(const QByteArray& array)
{
	QDataStream ds(array);
	QtMimeType data;
	ds >> data;
	return data;
}

QDataStream &operator<<(QDataStream &out, const QtMimeType &myObj)
{
	out << QString::number(reinterpret_cast<qulonglong>(myObj.ptr), 16);
	return out;
}

QDataStream &operator>>(QDataStream &in, QtMimeType &myObj)
{
	QString temp;
	in >> temp;
	myObj.ptr = reinterpret_cast<QtMultiPageWnd*>(temp.toULongLong(0, 16));
	return in;
}









