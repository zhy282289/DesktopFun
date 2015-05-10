#include "stdafx.h"
#include "desktopfun.h"
#include "controller.h"
#include "data.h"

#include "minimusicbox.h"

#include "widgetmovedragcontroller.h"

DesktopWindow::DesktopWindow( const QString &path, QWidget *parent)
	: QDialog(parent)
{
	setObjectName("DesktopWindow");
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_DeleteOnClose);
	//setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setMinimumSize(100, 100);
	::SetParent(winId(), FindDesktopWnd());

	setAcceptDrops(true);

	m_desktopController = new DesktopController(path, this);
	m_moveDragController = new WidgetMoveDragController(this);
	m_moveDragController->SetMoveable(m_desktopController->GetWindowMoveable());


	// animation
	m_animateWindow = new QPropertyAnimation(this, "pos", this);
	m_animateWindow->setDuration(1000);
	m_animateWindow->setEasingCurve(QEasingCurve::OutCubic);
	connect(m_animateWindow, SIGNAL(finished()), this, SLOT(SlotAnimationFinish()));

	// data
	m_bWindowHadMove = false;
	m_outside = 2;

	m_iconSize = QSize(g_iconSize, g_iconSize);


	QTimer *saveDeskTimer = new QTimer(this);
	saveDeskTimer->start(5 * 1000);
	connect(saveDeskTimer, SIGNAL(timeout()), this, SLOT(SlotSaveDesk()));

	m_actAddFile = new QAction("增加文件", this);
	m_actAddDir = new QAction("增加文件夹", this);
	m_actAnchorWindow = new QAction("窗口停泊", this);
	m_actAnchorWindow->setCheckable(true);
	m_actAnchorItem = new QAction("图标停泊", this);
	m_actAnchorItem->setCheckable(true);
	m_actAddNewWindow = new QAction("新窗口", this);
	m_actRemoveNewWindow = new QAction("删除窗口", this);
	m_actSetting = new QAction("设置", this);
	QList<QAction*> actObjs;
	actObjs << m_actAddFile << m_actAddDir << m_actAnchorWindow << m_actAnchorItem << m_actAddNewWindow << m_actRemoveNewWindow << m_actSetting ;
	foreach(QAction *obj, actObjs)
		connect(obj, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));	

}

DesktopWindow::~DesktopWindow()
{
	m_desktopController->Save();
}


void DesktopWindow::SetController( Controller *controller )
{
	//m_controller = controller;
	//controller->setParent(this);
	//m_items = m_controller->GetItems();
	//m_windowData = m_controller->GetDesktopWindowData();

	//// init ui
	//m_actAnchorWindow->setChecked(!m_windowData.bWindowCanMove);
	//m_actAnchorItem->setChecked(!m_windowData.bItemCanMove);
}

void DesktopWindow::paintEvent( QPaintEvent *event )
{
	if (!m_windowData.windowHadHide)
	{
		QPainter painter(this);

		//QColor color = m_windowData.color;
		//color.setAlpha(250);
		//painter.fillRect(rect(), QBrush(Qt::NoBrush));

		QPixmap desktopPixmap = !m_windowData.bgPixmap.bgPixmap.isNull() ?  m_windowData.bgPixmap.bgPixmap: QueryDesktopPixmap();
		painter.drawPixmap(rect(), desktopPixmap, geometry());

		painter.fillRect(rect(), QBrush(m_windowData.color));
	}
	else
	{
		QPainter painter(this);
		QColor color = m_windowData.color;
		painter.fillRect(rect(), QBrush(color));
	}

}

void DesktopWindow::mousePressEvent( QMouseEvent *event )
{
	if (m_windowData.windowHadHide)
		HideOrShowWindow(false);

}

void DesktopWindow::mouseReleaseEvent( QMouseEvent *event )
{
	//m_bLeftBtnDown = false;
	//QApplication::restoreOverrideCursor();
}

void DesktopWindow::mouseMoveEvent( QMouseEvent *event )
{
	update();
	m_bWindowHadMove = true;
	QDialog::mouseMoveEvent(event);
}


void DesktopWindow::showEvent( QShowEvent *event )
{
	QueryDesktopPixmap();
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
		AddFilesOrDirectoryDlg dlg(this);
		QStringList paths = dlg.AddFiles();
		for (int i = 0; i < paths.size(); ++i)
		{
			QPoint pos = obj->data().toPoint();
			ItemData data;
			data.rect = QRect(pos.x(), pos.y(), g_iconSize, g_iconSize);
			data.path = QDir::toNativeSeparators(paths.at(i));
			QByteArray pathArray = data.path.toLocal8Bit();
			data.pix = QueryExeIcon(pathArray.data());
			//data.bItemCanMove = m_windowData.bItemCanMove;
			m_desktopController->AddItem(data);
			//m_controller->Add(data, this);
		}
	}
	else if (m_actAddDir == obj)
	{
		//QString path = QFileDialog::getExistingDirectory(this, "增加文件夹");
		AddFilesOrDirectoryDlg dlg(this);
		QString path = dlg.AddDirectory();
		if (!path.isEmpty())
		{
			QPoint pos = obj->data().toPoint();
			ItemData data;
			data.rect = QRect(pos.x(), pos.y(), g_iconSize, g_iconSize);
			data.path = QDir::toNativeSeparators(path);
			QByteArray pathArray = data.path.toLocal8Bit();
			data.pix = QueryExeIcon(pathArray.data());
			//m_controller->Add(data, this);
			m_desktopController->AddItem(data);
		}
	}
	else if (m_actAnchorWindow == obj)
	{
		//m_windowData.bWindowCanMove = !m_actAnchorWindow->isChecked();
		
		m_moveDragController->SetMoveable(!m_actAnchorWindow->isChecked());
		m_desktopController->SetWindowMoveable(!m_actAnchorWindow->isChecked());
	}
	else if (m_actAnchorItem == obj)
	{
		//m_windowData.bItemCanMove = !m_actAnchorItem->isChecked();
		//m_controller->SetItemCanMove(m_windowData.bItemCanMove);
		m_desktopController->SetItemMoveable(!m_actAnchorItem->isChecked());
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
	menu.addSeparator();
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
	//Item *item = qobject_cast<Item*>(sender());
	//m_controller->Remove(item);

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
	if (m_bWindowHadMove)
	{
		m_desktopController->Save();
		m_bWindowHadMove = false;
	}
}

void DesktopWindow::SlotItemHadMove()
{
	m_bWindowHadMove = true;
}

//void DesktopWindow::SaveDesk()
//{
//	//m_windowData.rect = geometry();
//	m_desktopController->Save();
//	//m_controller->SetDesktopWindowData(m_windowData);
//	//m_controller->Save();
//}

void DesktopWindow::Show()
{
	setGeometry(m_desktopController->GetWindowGeometry());
	show();
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
		break;
	case Qt::Key_Delete:
		{
			RemoveThisWindow();
		}
		break;
	case Qt::Key_O:
		{
			if (event->modifiers() & Qt::AltModifier)
			{
				GetMiniMusicBox()->Show();
			}
		}
		break;
	}
}



void DesktopWindow::SlotAnimationFinish()
{
	update(rect());
	m_bWindowHadMove = true;
}

void DesktopWindow::dragEnterEvent( QDragEnterEvent *event )
{
	if (event->mimeData()->hasUrls())
		event->accept();
	else
		event->ignore();
	
}
void DesktopWindow::dragMoveEvent( QDragMoveEvent *event )
{
	if (event->mimeData()->hasUrls())
		event->accept();
	else
		event->ignore();
	
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
		//data.bItemCanMove = m_windowData.bItemCanMove;
		//m_controller->Add(data, this);
		m_desktopController->AddItem(data);
		pos.setX(pos.x() + g_iconSize + 10);
	}
}

void DesktopWindow::SlotSettingChanged( SettingData data )
{
	m_windowData.color = data.color;
	m_windowData.bgPixmap.bgPixmap = data.bgPix.bgPixmap;
	m_windowData.bgPixmap.path = data.bgPix.path;
	//m_controller->SetItemBGColor(data.color);
	update();
}

void DesktopWindow::RemoveThisWindow()
{
	int i = 0;
	for (;i < 2; ++i)
	{
		MsgBox msgBox(this);
		if (!msgBox.MsgQuestion("提示", "删除本窗口?"))
		{
			break;
		}
	}
	if (i == 2)
	{
		hide();
		m_desktopController->RemveWindow();
		//QFile::remove(m_desktopController->GetSavePath());
	}
}

void DesktopWindow::mouseDoubleClickEvent( QMouseEvent *event )
{
	HideOrShowWindow(true);
}






