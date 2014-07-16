#include "stdafx.h"
#include "desktopfun.h"
#include "controller.h"
#include "data.h"



DesktopWindow::DesktopWindow( QWidget *parent)
	: QDialog(parent)
{
	setObjectName("DesktopWindow");
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setMinimumSize(100, 100);
	setMouseTracking(true);

	::SetParent(winId(), FindDesktopWnd());

	setAcceptDrops(true);

	// animation
	m_animateWindow = new QPropertyAnimation(this, "pos", this);
	m_animateWindow->setDuration(1000);
	m_animateWindow->setEasingCurve(QEasingCurve::OutCubic);
	connect(m_animateWindow, SIGNAL(finished()), this, SLOT(SlotAnimationFinish()));

	// data
	QSettings settings("DesktopWindow.ini", QSettings::IniFormat);
	m_bLeftBtnDown = false;
	m_bMoveWindow = true;
	m_borderWidth = 4;
	m_bWindowHadMove = false;

	m_outside = 1;

	m_iconSize = QSize(g_iconSize, g_iconSize);


	m_saveIntevalTime = 5 * 1000;
	m_saveDeskTimer = new QTimer(this);
	m_saveDeskTimer->start(m_saveIntevalTime);
	connect(m_saveDeskTimer, SIGNAL(timeout()), this, SLOT(SlotSaveDesk()));

	m_actAddFile = new QAction("增加文件", this);
	m_actAddDir = new QAction("增加文件夹", this);
	m_actAnchorWindow = new QAction("窗口停泊", this);
	m_actAnchorWindow->setCheckable(true);
	m_actAnchorItem = new QAction("图标停泊", this);
	m_actAnchorItem->setCheckable(true);

	m_actAddNewWindow = new QAction("新窗口", this);
	m_actRemoveNewWindow = new QAction("删除窗口", this);
	m_actSetting = new QAction("设置", this);

	connect(m_actAddFile, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actAddDir, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actAnchorWindow, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actAnchorItem, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actAddNewWindow, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actSetting, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actRemoveNewWindow, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
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
	m_windowData = m_controller->GetDesktopWindowData();

	// init ui
	m_actAnchorWindow->setChecked(!m_windowData.bWindowCanMove);
	m_actAnchorItem->setChecked(!m_windowData.bItemCanMove);
}

void DesktopWindow::paintEvent( QPaintEvent *event )
{
	if (!m_windowData.windowHadHide)
	{
		QPixmap desktopPixmap = !m_windowData.bgPixmap.bgPixmap.isNull() ?  m_windowData.bgPixmap.bgPixmap: QueryDesktopPixmap();
		QRect srctRect = geometry();
		QRect destRect = rect();
		QPainter painter(this);
		painter.drawPixmap(destRect, desktopPixmap, srctRect);
		painter.fillRect(rect(), QBrush(m_windowData.color));
	}
	else
	{
		QPainter painter(this);
		QColor color = m_windowData.color;
		//color.setAlpha(250);
		painter.fillRect(rect(), QBrush(color));
	}

}

void DesktopWindow::mousePressEvent( QMouseEvent *event )
{
	if (event->button() == Qt::LeftButton && m_windowData.bWindowCanMove)
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
	if (m_windowData.windowHadHide)
	{
		HideOrShowWindow(false);
	}
}

void DesktopWindow::mouseReleaseEvent( QMouseEvent *event )
{
	m_bLeftBtnDown = false;
	QApplication::restoreOverrideCursor();
}

void DesktopWindow::mouseMoveEvent( QMouseEvent *event )
{
	if (m_bLeftBtnDown && (m_windowData.bWindowCanMove || WillHideWindow()))
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
	else if (m_windowData.bWindowCanMove && !m_windowData.windowHadHide)
	{
		HitSide(event);
	}
	QDialog::mouseMoveEvent(event);
}


void DesktopWindow::showEvent( QShowEvent *event )
{
	QueryDesktopPixmap();
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
		QStringList paths = QFileDialog::getOpenFileNames(this, "增加文件");
		for (int i = 0; i < paths.size(); ++i)
		{
			QPoint pos = obj->data().toPoint();
			ItemData data;
			data.rect = QRect(pos.x(), pos.y(), g_iconSize, g_iconSize);
			data.path = QDir::toNativeSeparators(paths.at(i));
			QByteArray pathArray = data.path.toLocal8Bit();
			data.pix = QueryExeIcon(pathArray.data());
			data.bItemCanMove = m_windowData.bItemCanMove;
			m_controller->Add(data, this);
		}
	}
	else if (m_actAddDir == obj)
	{
		QString path = QFileDialog::getExistingDirectory(this, "增加文件夹");
		if (!path.isEmpty())
		{
			QPoint pos = obj->data().toPoint();
			ItemData data;
			data.rect = QRect(pos.x(), pos.y(), g_iconSize, g_iconSize);
			data.path = QDir::toNativeSeparators(path);
			QByteArray pathArray = data.path.toLocal8Bit();
			data.pix = QueryExeIcon(pathArray.data());
			m_controller->Add(data, this);
		}
	}
	else if (m_actAnchorWindow == obj)
	{
		m_windowData.bWindowCanMove = !m_actAnchorWindow->isChecked();
	}
	else if (m_actAnchorItem == obj)
	{
		m_windowData.bItemCanMove = !m_actAnchorItem->isChecked();
		m_controller->SetItemCanMove(m_windowData.bItemCanMove);
	}
	else if (m_actAddNewWindow == obj)
	{
		CreateNewDesktopWindow();
	}
	else if (m_actRemoveNewWindow == obj)
	{
		RemoveThisWindow();

	}
	else if (m_actSetting == obj)
	{
		SettingsDlg *settingDlg = new SettingsDlg(this);
		connect(settingDlg, SIGNAL(SigSettingChanged(SettingData)), this, SLOT(SlotSettingChanged(SettingData)), Qt::UniqueConnection);

		SettingData data;
		data.color = m_windowData.color;
		data.bgPix.bgPixmap = m_windowData.bgPixmap.bgPixmap;
		data.bgPix.path = m_windowData.bgPixmap.path;
		settingDlg->Show(data);
	}
}

void DesktopWindow::contextMenuEvent( QContextMenuEvent *event )
{
	//if (WillHideWindow())
	//	return;


	QMenu menu;
	menu.addAction(m_actAddFile);
	menu.addAction(m_actAddDir);
	menu.addAction(m_actAnchorWindow);
	menu.addAction(m_actAnchorItem);
	menu.addAction(m_actAddNewWindow);
	menu.addAction(m_actRemoveNewWindow);
	menu.addSeparator();
	menu.addAction(m_actSetting);

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
	m_windowData.bWindowCanMove = canMove;
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
	m_windowData.color = color;
	update();
}

QColor DesktopWindow::GetBgColor()
{
	return m_windowData.color;
}

void DesktopWindow::SlotSaveDesk()
{
	if (m_bWindowHadMove /*&& m_windowData.bWindowCanMove*/)
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
	m_windowData.rect = geometry();
	m_controller->SetDesktopWindowData(m_windowData);
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
	//HideOrShowWindow(false);
}

void DesktopWindow::HideOrShowWindow( bool hide )
{
	QRect screenGeometry = qApp->desktop()->availableGeometry();
	bool left = geometry().x() <= 0;
	bool top = geometry().y() <= 0;
	bool right = geometry().right() >= screenGeometry.width();
	bool bottom = geometry().bottomLeft().y() >= screenGeometry.height();

	if (!left && !top && !right && !bottom)
		return;

	m_windowData.windowHadHide = hide;

	int ileft = geometry().x();
	int itop = geometry().y();
	int iright = screenGeometry.width() - geometry().right();
	int ibottom = screenGeometry.height() - geometry().bottomLeft().y() ;

	left = ileft < itop && ileft < ibottom && ileft < iright;
	top = itop < ileft && itop < iright && itop < ibottom;
	right = iright < itop && iright < ibottom && iright < ileft;
	bottom= ibottom < iright && ibottom < ileft && ibottom < itop;
	if (hide)
	{
		//  隐藏窗体动画
		QPoint pos = geometry().topLeft();
		if (left)
		{
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(-geometry().width() + m_outside, pos.y()));
			m_animateWindow->start();
			//m_handleWidget->Move(QPoint(0, 100));
		}
		else if (top)
		{
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(pos.x(), -geometry().height() + m_outside));
			m_animateWindow->start();
		}
		else if (right)
		{
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(screenGeometry.width() - m_outside, pos.y()));
			m_animateWindow->start();
		}
		else if (bottom)
		{
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(pos.x(), screenGeometry.height()-5));
			m_animateWindow->start();
		}
	}
	else 
	{
		QPoint pos = geometry().topLeft();
		if (left)
		{
			
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(qMin(-m_outside, screenGeometry.width() - geometry().width()), pos.y()));
			m_animateWindow->start();
		}
		else if (top)
		{
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(pos.x(), qMin(-m_outside, screenGeometry.height() - geometry().height())));
			m_animateWindow->start();
		}
		else if (right)
		{
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(qMax(0, screenGeometry.width() - geometry().width() + m_outside), pos.y()));
			m_animateWindow->start();
		}
		else if (bottom)
		{
			m_animateWindow->setStartValue(pos);
			m_animateWindow->setEndValue(QPoint(pos.x(), qMax(0, screenGeometry.height() - geometry().height()) + m_outside));
			m_animateWindow->start();
		}
	}

	setFocus();
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

void DesktopWindow::keyPressEvent( QKeyEvent *event )
{
	switch (event->key())
	{
	case Qt::Key_H:
		{
			HideOrShowWindow(!m_windowData.windowHadHide);
		}
		break;
	case Qt::Key_S:
		{
			HideOrShowWindow(false);
		}
	case Qt::Key_Delete:
		{
			RemoveThisWindow();
		}
		break;
	case Qt::Key_O:
		{
			if (event->modifiers() & Qt::AltModifier)
			{
				AboutDlg *aboutDlg = new AboutDlg(NULL);
				aboutDlg->resize(560, 300);
				aboutDlg->show();
			}
		}
		break;
	}
}

void DesktopWindow::focusOutEvent( QFocusEvent *event )
{
	//HideOrShowWindow(true);
}

void DesktopWindow::focusInEvent( QFocusEvent *event )
{

}

void DesktopWindow::SlotAnimationFinish()
{
	update(rect());
	m_bWindowHadMove = true;
}

void DesktopWindow::dragEnterEvent( QDragEnterEvent *event )
{
	if (event->mimeData()->hasUrls())
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}
void DesktopWindow::dragMoveEvent( QDragMoveEvent *event )
{
	if (event->mimeData()->hasUrls())
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}
void DesktopWindow::dropEvent( QDropEvent *event )
{
	QList<QUrl> urls = event->mimeData()->urls();
	QPoint pos = event->pos();
	for (int i = 0; i < urls.size(); ++i)
	{
		ItemData data;
		data.rect = QRect(pos.x(), pos.y(), g_iconSize, g_iconSize);
		data.path = QDir::toNativeSeparators(urls.at(i).toLocalFile());
		QByteArray pathArray = data.path.toLocal8Bit();
		data.pix = QueryExeIcon(pathArray.data());
		data.bItemCanMove = m_windowData.bItemCanMove;
		m_controller->Add(data, this);

		pos.setX(pos.x() + g_iconSize + 10);
	}
}

void DesktopWindow::SlotSettingChanged( SettingData data )
{
	m_windowData.color = data.color;
	m_windowData.bgPixmap.bgPixmap = data.bgPix.bgPixmap;
	m_windowData.bgPixmap.path = data.bgPix.path;
	m_controller->SetItemBGColor(data.color);
	update();
}

void DesktopWindow::RemoveThisWindow()
{
	int i = 0;
	for (;i < 2; ++i)
	{
		if (!(QMessageBox::Yes == QMessageBox::warning(this, "提示", "删除本窗口?", QMessageBox::Yes | QMessageBox::No)))
		{
			break;
		}
	}
	if (i == 2)
	{
		hide();
		QFile::remove(m_controller->GetSavePath());
	}
}

void DesktopWindow::mouseDoubleClickEvent( QMouseEvent *event )
{
	HideOrShowWindow(true);
}










QDataStream &operator<<(QDataStream &out, const DesktopWindowData &data)
{
	return out << data.rect << data.bWindowCanMove << data.bItemCanMove << data.windowHadHide << data.color << data.bgPixmap.path;
}
QDataStream &operator>>(QDataStream &in, DesktopWindowData &data)
{
	return in >> data.rect >> data.bWindowCanMove >> data.bItemCanMove >> data.windowHadHide >> data.color >> data.bgPixmap.path;
}

HandleWidget::HandleWidget( QWidget *parent /*= NULL*/ )
	:QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint);
	setFixedSize(15, 30);
	setFocusPolicy(Qt::NoFocus);
	m_bgColor = QColor(0, 250, 250, 50);
}

void HandleWidget::paintEvent( QPaintEvent *event )
{
	QPixmap desktopPixmap = QueryDesktopPixmap();
	QRect srctRect = geometry();
	QRect destRect = rect();
	QPainter painter(this);
	painter.drawPixmap(destRect, desktopPixmap, srctRect);
	painter.fillRect(rect(), QBrush(m_bgColor));
}

void HandleWidget::mousePressEvent( QMouseEvent *event )
{
	Hide();
	emit clicked();
}

void HandleWidget::Move( QPoint point )
{
	move(point);
	show();
}

void HandleWidget::Hide()
{
	hide();
}
