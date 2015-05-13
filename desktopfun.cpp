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
	//setAttribute(Qt::WA_OpaquePaintEvent);
	setMinimumSize(100, 100);
	::SetParent(winId(), FindDesktopWnd());

	setAcceptDrops(true);

	// animation
	m_animateWindow = new QPropertyAnimation(this, "pos", this);
	m_animateWindow->setDuration(1000);
	m_animateWindow->setEasingCurve(QEasingCurve::OutCubic);
	connect(m_animateWindow, SIGNAL(finished()), this, SLOT(SlotAnimationFinish()));

	// data
	m_outside = 3;


	m_actAddFile = new QAction(tr("add shortcut"), this);
	m_actAddDir = new QAction(tr("add directory"), this);
	m_actAnchorWindow = new QAction(tr("window anchor"), this);
	m_actAnchorWindow->setCheckable(true);
	m_actAnchorItem = new QAction(tr("item anchor"), this);
	m_actAnchorItem->setCheckable(true);
	m_actAddNewWindow = new QAction(tr("new window"), this);
	m_actRemoveNewWindow = new QAction(tr("remove window"), this);
	m_actSetting = new QAction(tr("settings"), this);
	QList<QAction*> actObjs;
	actObjs << m_actAddFile << m_actAddDir << m_actAnchorWindow << m_actAnchorItem << m_actAddNewWindow << m_actRemoveNewWindow << m_actSetting ;
	foreach(QAction *obj, actObjs)
		connect(obj, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));	


	m_desktopController = new DesktopController(path, this);
	m_moveDragController = new WidgetMoveDragController(this);
	m_moveDragController->SetMoveable(m_desktopController->GetWindowMoveable());
	m_moveDragController->SetDragable(m_desktopController->GetWindowMoveable());
}

DesktopWindow::~DesktopWindow()
{
	
}


void DesktopWindow::paintEvent( QPaintEvent *event )
{
	//if (m_desktopController->IsWindowVisiable())
	//{
		QPainter painter(this);
		QPixmap desktopPixmap = m_desktopController->GetWindowBgPixmap();
		painter.drawPixmap(rect(), desktopPixmap, geometry());
		painter.fillRect(rect(), QBrush(m_desktopController->GetWindowBgColor()));
	//}
	//else
	//{
	//	QPainter painter(this);
	//	QColor color = m_desktopController->GetWindowBgColor();
	//	painter.fillRect(rect(), QBrush(color));
	//}

}

void DesktopWindow::mousePressEvent( QMouseEvent *event )
{
	if (!m_desktopController->IsWindowVisiable())
		HideOrShowWindow(false);
	m_desktopController->SetWindowVisiable(true);
}

void DesktopWindow::mouseReleaseEvent( QMouseEvent *event )
{
	m_desktopController->SaveState();
}

void DesktopWindow::mouseMoveEvent( QMouseEvent *event )
{
	update();
	QDialog::mouseMoveEvent(event);
}


//void DesktopWindow::showEvent( QShowEvent *event )
//{
//	QueryDesktopPixmap();
//}

static const ItemData& sGetItemDataFromPos(QPoint pos, const QString &path)
{
	static ItemData data;
	data.id == QUuid::createUuid().toString();
	data.rect = QRect(pos.x(), pos.y(), g_iconSize, g_iconSize);
	data.path = QDir::toNativeSeparators(path);
	QByteArray pathArray = data.path.toLocal8Bit();
	data.pix = QueryExeIcon(pathArray.data());
	return data;
}

void DesktopWindow::SlotActTriggered()
{
	QAction *obj = qobject_cast<QAction*>(sender());
	if (obj == m_actAddFile)
	{
		AddFilesOrDirectoryDlg dlg(this);
		QStringList paths = dlg.AddFiles();
		for (int i = 0; i < paths.size(); ++i)
			m_desktopController->AddItem(sGetItemDataFromPos(obj->data().toPoint(), paths.at(i)));
		
	}
	else if (m_actAddDir == obj)
	{
		AddFilesOrDirectoryDlg dlg(this);
		QString path = dlg.AddDirectory();
		if (!path.isEmpty())
			m_desktopController->AddItem(sGetItemDataFromPos(obj->data().toPoint(), path));
		
	}
	else if (m_actAnchorWindow == obj)
	{	
		m_moveDragController->SetDragable(!m_actAnchorWindow->isChecked());
		m_moveDragController->SetMoveable(!m_actAnchorWindow->isChecked());
		m_desktopController->SetWindowMoveable(!m_actAnchorWindow->isChecked());
		m_desktopController->SaveState();
	}
	else if (m_actAnchorItem == obj)
	{
		
		m_desktopController->SetItemMoveable(!m_actAnchorItem->isChecked());
		m_desktopController->SaveState();
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
		data.color = m_desktopController->GetWindowBgColor();
		data.bgPix.bgPixmap = m_desktopController->GetWindowBgPixmap();
		settingDlg->Show(data);
	}
}

void DesktopWindow::contextMenuEvent( QContextMenuEvent *event )
{
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

void DesktopWindow::keyPressEvent( QKeyEvent *event )
{
	switch (event->key())
	{
	case Qt::Key_H:
		{
			HideOrShowWindow(!m_desktopController->IsWindowVisiable());
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
	case Qt::Key_V:
		{
			if (event->modifiers() & Qt::AltModifier)
			{
				QLabel *lb = new QLabel();
				lb->resize(450, 150);
				lb->setAttribute(Qt::WA_DeleteOnClose);
				lb->setStyleSheet("QLabel{padding:20px 20px 20px 30px}");
				lb->setText("creator:2015.05.11\nby:BOY\nGNU General Public Licence");
				lb->show();
			}
		}
	}
}



void DesktopWindow::SlotAnimationFinish()
{
	update(rect());
	m_desktopController->SaveState();
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
		pos.setX(pos.x() + g_iconSize + 10);
		m_desktopController->AddItem(sGetItemDataFromPos(pos, urls.at(i).toLocalFile()));
	}
}

void DesktopWindow::SlotSettingChanged( SettingData data )
{
	m_desktopController->SetWindowBgColor(data.color);
	BGPixmap bgPixmap;
	bgPixmap.bgPixmap = data.bgPix.bgPixmap;
	bgPixmap.path = data.bgPix.path;
	m_desktopController->SetWindowBgPixmap(bgPixmap);
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
		m_desktopController->RemoveWindow();
	}
}

void DesktopWindow::mouseDoubleClickEvent( QMouseEvent *event )
{
	HideOrShowWindow(true);
	m_desktopController->SetWindowVisiable(false);
}







