#include "stdafx.h"
#include "desktopfun.h"
#include "controller.h"
#include "data.h"



DesktopWindow::DesktopWindow( QWidget *parent)
	: QDialog(parent)
{
	
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumSize(100, 100);
	setMouseTracking(true);

	//::SetParent(winId(), FindDesktopWnd());

	// animation
	m_animateWindow = new QPropertyAnimation(this, "pos", this);
	m_animateWindow->setDuration(1000);
	m_animateWindow->setEasingCurve(QEasingCurve::OutCubic);

	// data
	QSettings settings("DesktopWindow.ini", QSettings::IniFormat);
	m_bLeftBtnDown = false;
	m_bMoveWindow = true;
	m_borderWidth = 4;
	m_bWindowCanMove = settings.value("WindowCanMove", 1).toBool();
	m_bItemCanMove = settings.value("ItemCanMove", 1).toBool();
	m_bWindowHadMove = false;
	

	m_iconSize = QSize(80, 80);
	m_bgColor = QColor(0, 0, 250, 80);

	m_saveIntevalTime = 5 * 1000;
	m_saveDeskTimer = new QTimer(this);
	m_saveDeskTimer->start(m_saveIntevalTime);
	connect(m_saveDeskTimer, SIGNAL(timeout()), this, SLOT(SlotSaveDesk()));

	m_actAddFile = new QAction("增加文件", this);
	m_actAddDir = new QAction("增加文件夹", this);
	m_actAnchor = new QAction("停泊", this);
	m_actAnchor->setCheckable(true);
	m_actAnchor->setChecked(!m_bWindowCanMove);

	
	

	connect(m_actAddFile, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actAddDir, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actAnchor, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
}

DesktopWindow::~DesktopWindow()
{
	SaveDesk();
}


void DesktopWindow::SetController( Controller *controller )
{
	m_controller = controller;
	controller->setParent(this);
	m_items = m_controller->GetItems();
}

void DesktopWindow::paintEvent( QPaintEvent *event )
{
	QPixmap desktopPixmap = QueryDesktopPixmap();
	QRect destRect = geometry();
	QRect srctRect = rect();
	QPainter painter(this);
	painter.drawPixmap(srctRect, desktopPixmap, destRect);
	painter.fillRect(rect(), QBrush(m_bgColor));
}

void DesktopWindow::mousePressEvent( QMouseEvent *event )
{
	if (event->button() == Qt::LeftButton && m_bWindowCanMove)
	{
		m_bLeftBtnDown = true;
		m_oldPoint = event->pos();

		QRect moveWindowRect = rect().adjusted(m_borderWidth, m_borderWidth, -m_borderWidth, -m_borderWidth);
		m_bMoveWindow = moveWindowRect.contains(m_oldPoint);
		if (m_bMoveWindow)
		{
			QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		}
	}

}

void DesktopWindow::mouseReleaseEvent( QMouseEvent *event )
{
	m_bLeftBtnDown = false;
	QApplication::restoreOverrideCursor();
}

void DesktopWindow::mouseMoveEvent( QMouseEvent *event )
{
	if (m_bLeftBtnDown && (m_bWindowCanMove || WillHideWindow()))
	{
		if (m_bMoveWindow)
		{
			// 移动 窗口
			move(event->globalPos() - m_oldPoint);
			
		}
		else
		{
			// 拖拽窗口
			QPoint newPoint = (event->pos());
			MoveWindow(m_oldPoint, newPoint);		
			m_oldPoint = newPoint;
		}
		update();
		m_bWindowHadMove = true;
	}
	else if (m_bWindowCanMove)
	{
		qDebug() << MouseInBorder(event->pos()) << !AnimationIsRun();
		if (MouseInBorder(event->pos()) && !AnimationIsRun())
		{
			HideOrShowWindow(true);
		}
		else
		{
			HitSide(event);
		}
		
	}
}
void DesktopWindow::showEvent( QShowEvent *event )
{
	//for (int i = 0; i < m_items->size(); ++i)
	//{
	//	m_items->at(i)->UpdatePostion();
	//}
}

void DesktopWindow::HitSide( QMouseEvent *event )
{
	left = top = right = bottom = false;
	const int margin = m_borderWidth;
	QRect outRect = this->rect();
	QPoint pos = event->pos();
	int x = pos.x();
	int y = pos.y();
	if (x < margin)
		left = true;
	if (y < margin)
		top = true;
	if (x > outRect.right()-margin)
		right = true;
	if (y > outRect.bottom()-margin)
		bottom = true;

	if (left && top)
		this->setCursor(Qt::SizeFDiagCursor);
	else if (left && bottom)
		this->setCursor(Qt::SizeBDiagCursor);
	else if (right && top)
		this->setCursor(Qt::SizeBDiagCursor);
	else if (right && bottom)
		this->setCursor(Qt::SizeFDiagCursor);
	else if (left || right)
		this->setCursor(Qt::SizeHorCursor);
	else if (top || bottom)
		this->setCursor(Qt::SizeVerCursor);
	else
		this->setCursor(Qt::ArrowCursor);

}

void DesktopWindow::MoveWindow( const QPoint &oldPoint, const QPoint &newPoint )
{
	int dx = newPoint.x() - oldPoint.x();
	int dy = newPoint.y() - oldPoint.y();

	QRect rect = this->geometry();

	if (left)
		rect.setLeft(rect.x() + dx);
	if (top)
		rect.setTop(rect.y() + dy);
	if (right)
		rect.setRight(rect.x()+ rect.width()-1 + dx);
	if (bottom)
		rect.setBottom(rect.y() + rect.height()-1 + dy);
	
	setGeometry(rect);
}

void DesktopWindow::SlotOpenProcess( QString path )
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void DesktopWindow::SlotActTriggered()
{
	QAction *obj = qobject_cast<QAction*>(sender());
	if (obj == m_actAddFile)
	{
		QStringList paths = QFileDialog::getOpenFileNames(this, "增加");
		for (int i = 0; i < paths.size(); ++i)
		{
			QPoint pos = obj->data().toPoint();
			ItemData data;
			data.rect = QRect(pos.x(), pos.y(), 80, 80);
			data.path = QDir::toNativeSeparators(paths.at(i));
			QByteArray pathArray = data.path.toLocal8Bit();
			data.pix = QueryExeIcon(pathArray.data());
			m_controller->Add(data, this);
		}
	}
	else if (m_actAddDir == obj)
	{
		QString path = QFileDialog::getExistingDirectory(this, "增加");
		if (!path.isEmpty())
		{
			QPoint pos = obj->data().toPoint();
			ItemData data;
			data.rect = QRect(pos.x(), pos.y(), 80, 80);
			data.path = QDir::toNativeSeparators(path);
			QByteArray pathArray = data.path.toLocal8Bit();
			data.pix = QueryExeIcon(pathArray.data());
			m_controller->Add(data, this);
		}
	}
	else if (m_actAnchor == obj)
	{
		m_bWindowCanMove = !m_actAnchor->isChecked();
		m_bItemCanMove = !m_actAnchor->isChecked();
		m_controller->SetItemCanMove(m_bItemCanMove);

		QSettings settings("DesktopWindow.ini", QSettings::IniFormat);
		settings.setValue("WindowCanMove", m_bWindowCanMove);
		settings.setValue("ItemCanMove", m_bItemCanMove);
	}
}

void DesktopWindow::contextMenuEvent( QContextMenuEvent *event )
{
	if (WillHideWindow())
		return;


	QMenu menu;
	menu.addAction(m_actAddFile);
	menu.addAction(m_actAddDir);
	menu.addSeparator();
	menu.addAction(m_actAnchor);

	m_actAddFile->setData(event->pos());
	m_actAddDir->setData(event->pos());
	menu.exec(event->globalPos());
}

void DesktopWindow::SlotItemMenu( int menuID )
{
	Item *item = qobject_cast<Item*>(sender());
	m_controller->Remove(item);

}

void DesktopWindow::SetCanMove( bool canMove )
{
	m_bWindowCanMove = canMove;
}

void DesktopWindow::SetIconSize( QSize size )
{
	m_iconSize = size;
}

QSize DesktopWindow::GetIconSize()
{
	return m_iconSize;
}

void DesktopWindow::SetBgColor( QColor color )
{
	m_bgColor = color;
	update();
}

QColor DesktopWindow::GetBgColor()
{
	return m_bgColor;
}

void DesktopWindow::SlotSaveDesk()
{
	if (m_bWindowHadMove && m_bWindowCanMove)
	{
		SaveDesk();
		m_bWindowHadMove = false;
	}
	
}

void DesktopWindow::SlotItemHadMove()
{
	m_bWindowHadMove = true;
}

void DesktopWindow::SaveDesk()
{
	DesktopWindowData data;
	data.rect = geometry();
	m_controller->SetDesktopWindowData(data);
	m_controller->Save();
}

void DesktopWindow::Show()
{

	setGeometry(m_controller->GetDesktopWindowData().rect);
	show();
}

void DesktopWindow::leaveEvent( QEvent *event )
{
	//HideOrShowWindow(true);
}

void DesktopWindow::enterEvent( QEvent *event )
{
	HideOrShowWindow(false);
}

void DesktopWindow::HideOrShowWindow( bool hide )
{
	bool left = geometry().x() <= 0;
	bool right = geometry().y() <= 0;
	bool top = geometry().right() >= qApp->desktop()->availableGeometry().width();
	if (hide)
	{
		QPoint pos = geometry().topLeft();
		if (left)
		{
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(-geometry().width()+5, pos.y()));
			m_animateWindow->start();
		}
	}
	else 
	{
		QPoint pos = geometry().topLeft();
		if (left)
		{
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(-2, pos.y()));
			m_animateWindow->start();
		}

	}
}

bool DesktopWindow::WillHideWindow()
{
	bool left = geometry().x() <= 0;
	bool right = geometry().y() <= 0;
	bool top = geometry().right() >= qApp->desktop()->availableGeometry().width();

	return left | right | top;
}

bool DesktopWindow::AnimationIsRun()
{
	return m_animateWindow->state() == QAbstractAnimation::Running;
}

bool DesktopWindow::MouseInBorder(QPoint point)
{
	const int margin = m_borderWidth;
	QRect outRect = this->rect();
	QPoint pos = point;
	int x = pos.x();
	int y = pos.y();
	if (x < margin)
		left = true;
	if (y < margin)
		top = true;
	if (x > outRect.right()-margin)
		right = true;
	
	return left | right | top;

}




QDataStream &operator<<(QDataStream &out, const DesktopWindowData &data)
{
	return out << data.rect;
}
QDataStream &operator>>(QDataStream &in, DesktopWindowData &data)
{
	return in >> data.rect;
}