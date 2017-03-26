#include "stdafx.h"
#include "controller.h"
#include "data.h"

//////////////////////////////////////////////////////////////////////////
DesktopController::DesktopController( const QString path, QWidget *parent )
	:QObject(parent)
	,m_parent((DesktopWindow*)parent)
{
	Q_ASSERT(qobject_cast<DesktopWindow*>(parent));
	Load(path);

	// init desktop
	m_parent->m_actAnchorWindow->setChecked(!m_desktopWindowData.bWindowCanMove);
	m_parent->m_actAnchorItem->setChecked(!m_desktopWindowData.bItemCanMove);
}

bool DesktopController::Load( const QString &path )
{
	m_savePath = path;

	QFile file(m_savePath);
	if (file.open(QIODevice::ReadOnly))
	{
		QList<ItemData> itemDatas;
		QDataStream stream(&file);
		stream >> m_desktopWindowData;
		QPixmap pixmap;
		pixmap.load(m_desktopWindowData.bgPixmap.path);
		m_desktopWindowData.bgPixmap.bgPixmap = pixmap;

		for (int i = 0; i < m_desktopWindowData.Items.size(); ++i)
			_AddItem(m_desktopWindowData.Items[i]);
		
		return true;
	}

	return false;
}


void DesktopController::AddItem( const ItemData &item )
{
	m_desktopWindowData.Items.push_back(item);
	_AddItem(m_desktopWindowData.Items.last());
}
void DesktopController::_AddItem( ItemData &item )
{
	ItemEx *w = new ItemEx(&item,m_desktopWindowData.color, &m_desktopWindowData.bItemCanMove, m_parent);
	w->SetGeometry();
	connect(w, SIGNAL(DeleteLater(QString)), this, SLOT(DeleteLaterSlot(QString)));
	connect(w, SIGNAL(WindowStateChanged()), this, SLOT(SaveState()));
}

void DesktopController::SetWindowMoveable( bool moveable )
{
	m_desktopWindowData.bWindowCanMove = moveable;
}

bool DesktopController::GetWindowMoveable()
{
	return m_desktopWindowData.bWindowCanMove;
}

void DesktopController::SetItemMoveable( bool moveable )
{
	m_desktopWindowData.bItemCanMove = moveable;
}

bool DesktopController::GetItemMoveable()
{
	return m_desktopWindowData.bItemCanMove;
}

void DesktopController::RemoveWindow()
{
	QFile::remove(m_savePath);
}

void DesktopController::SaveState()
{
	if (m_savePath.isEmpty())
		m_savePath = GetPageSettingPath() + GetDefaultSaveDeskName();
	
	QFile file(m_savePath);
	if (file.open(QIODevice::WriteOnly))
	{
		m_desktopWindowData.rect = m_parent->geometry();
		QDataStream stream(&file);
		stream << m_desktopWindowData;
	}
}

QRect DesktopController::GetWindowGeometry()
{
	return m_desktopWindowData.rect;
}

void DesktopController::DeleteLaterSlot( QString id )
{
	for (int i = 0; i < m_desktopWindowData.Items.size(); ++i)
	{
		if (m_desktopWindowData.Items.at(i).id == id)
		{
			m_desktopWindowData.Items.removeAt(i);
			break;
		}
	}
}

BGPixmap DesktopController::GetWindowBgPixmap()
{
	BGPixmap bgPixmap;
	bgPixmap.bgPixmap = !m_desktopWindowData.bgPixmap.bgPixmap.isNull() ?  m_desktopWindowData.bgPixmap.bgPixmap: QueryDesktopPixmap();
	bgPixmap.path = m_desktopWindowData.bgPixmap.path;
	return bgPixmap;
	//return !m_desktopWindowData.bgPixmap.bgPixmap.isNull() ?  m_desktopWindowData.bgPixmap.bgPixmap: QueryDesktopPixmap();
}

bool DesktopController::IsWindowVisiable()
{
	return m_desktopWindowData.windowHadHide;
}

void DesktopController::SetWindowVisiable( bool visiable )
{
	m_desktopWindowData.windowHadHide = visiable;
}

QColor DesktopController::GetWindowBgColor()
{
	return m_desktopWindowData.color;
}

void DesktopController::SetWindowBgPixmap( const BGPixmap &pixmap )
{
	m_desktopWindowData.bgPixmap = pixmap;
}

void DesktopController::SetWindowBgColor( const QColor &color )
{
	m_desktopWindowData.color = color;
}
