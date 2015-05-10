#include "stdafx.h"
#include "controller.h"
#include "data.h"

//
//Controller::Controller(QObject *parent)
//	: QObject(parent)
//{
//
//}
//
//Controller::~Controller()
//{
//
//}
//
//QList<Item*> *Controller::Load( const QString &filePath, QWidget *parent)
//{
//	m_savePath = filePath;
//	
//	QFile file(m_savePath);
//	if (file.open(QIODevice::ReadOnly))
//	{
//		QList<ItemData> itemDatas;
//		QDataStream stream(&file);
//		stream >> m_desktopWindowData;
//		QPixmap pixmap;
//		pixmap.load(m_desktopWindowData.bgPixmap.path);
//		m_desktopWindowData.bgPixmap.bgPixmap = pixmap;
//		stream >> itemDatas;
//		for (int i = 0; i < itemDatas.size(); ++i)
//		{
//			Add(itemDatas.at(i), parent);
//			
//		}
//	}
//
//	return &m_items;
//}
//
//void Controller::Save()
//{
//	if (m_savePath.isEmpty())
//	{
//		m_savePath = GetPageSettingPath() + GetDefaultSaveDeskName();
//	}
//	QFile file(m_savePath);
//	if (file.open(QIODevice::WriteOnly))
//	{
//		QList<ItemData> itemDatas;
//		for (int i = 0; i < m_items.size(); ++i)
//		{
//			ItemData itemData = m_items.at(i)->GetData();
//			itemData.rect = m_items.at(i)->geometry();
//			itemDatas.push_back(itemData);
//		}
//
//		QDataStream stream(&file);
//		stream << m_desktopWindowData;
//		stream << itemDatas;
//	}
//}
//void Controller::Add(const ItemData &data, QWidget *parent)
//{
//	Item *w = new Item(parent);
//	w->SetData(data);
//	w->SetBGColor(m_desktopWindowData.color);
//	m_items.push_back(w);
//	w->UpdatePostion();
//
//	connect(w, SIGNAL(SigLeftMouseClicked(QString)), parent, SLOT(SlotOpenProcess(QString)), Qt::UniqueConnection);
//	connect(w, SIGNAL(SigItemMenu(int)), parent, SLOT(SlotItemMenu(int)), Qt::UniqueConnection);
//	connect(w, SIGNAL(SigItemHadMove()), parent, SLOT(SlotItemHadMove()), Qt::UniqueConnection);
//}
//
////void Controller::Add( const QString &path, QWidget *parent )
////{
////	ItemData data;
////	data.rect = QRect(50, 50, 80, 80);
////	data.path = QDir::toNativeSeparators(path);
////	QByteArray pathArray = data.path.toLocal8Bit();
////	data.pix = QueryExeIcon(pathArray.data());
////	Item *w = new Item(parent);
////	w->SetData(data);
////	m_items.push_back(w);
////	w->UpdatePostion();
////
////	connect(w, SIGNAL(SigLeftMouseClicked(QString)), parent, SLOT(SlotOpenProcess(QString)), Qt::UniqueConnection);
////	connect(w, SIGNAL(SigItemMenu(int)), parent, SLOT(SlotItemMenu(int)), Qt::UniqueConnection);
////}
//
//void Controller::Remove( Item *item )
//{
//	m_items.removeOne(item);
//	item->setParent(NULL);
//	item->deleteLater();
//}
//
//
//QList<Item*> * Controller::GetItems()
//{
//	return &m_items;
//}
//
//DesktopWindowData Controller::GetDesktopWindowData()
//{
//	return m_desktopWindowData;
//}
//
//void Controller::SetDesktopWindowData( const DesktopWindowData &data )
//{
//	m_desktopWindowData = data;
//}
//
//void Controller::SetItemCanMove( bool move )
//{
//	for (int i = 0; i < m_items.size(); ++i)
//	{
//		m_items.at(i)->SetCanMove(move);
//	}
//}
//
//
//
//QString Controller::GetSavePath()
//{
//	return m_savePath;
//}
//
//void Controller::SetItemBGColor( QColor color )
//{
//	int alpha = color.alpha()+50;
//	if (alpha >= 255)
//	{
//		color.setRed(qAbs(255 - color.red()));
//		color.setGreen(qAbs(255 - color.green()));
//		color.setBlue(qAbs(255 - color.blue()));
//		alpha = 200;
//	}
//
//	color.setAlpha(alpha);
//	for (int i = 0; i < m_items.size(); ++i)
//	{
//		m_items.at(i)->SetBGColor(color);
//	}
//}
//
//void Controller::RemoveAll()
//{
//	for (int i = 0; i < m_items.size(); ++i)
//	{
//		Remove(m_items.at(i));
//	}
//
//}
//
//
//Controller *g_controller = NULL;
//Controller* GetController( QObject *parent /*= NULL*/ )
//{
//	if (g_controller == NULL)
//	{
//		g_controller = new Controller(parent);
//	}
//	return g_controller;
//}
//

//////////////////////////////////////////////////////////////////////////
DesktopController::DesktopController( const QString path, QWidget *parent )
	:QObject(parent)
	,m_parent(parent)
{
	Q_ASSERT(parent);
	Load(path);
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

void DesktopController::_AddItem( ItemData &item )
{
	ItemEx *w = new ItemEx(&item, m_parent);
	//w->SetData(m_desktopWindowData.Items.at(i));
	w->SetBGColor(m_desktopWindowData.color);
	w->SetCanMove(&m_desktopWindowData.bItemCanMove);
	//m_items.push_back(w);
	w->UpdatePostion();
	//connect(w, SIGNAL(SigLeftMouseClicked(QString)), m_parent, SLOT(SlotOpenProcess(QString)), Qt::UniqueConnection);
	//connect(w, SIGNAL(SigItemMenu(int)), m_parent, SLOT(SlotItemMenu(int)), Qt::UniqueConnection);
	//connect(w, SIGNAL(SigItemHadMove()), m_parent, SLOT(SlotItemHadMove()), Qt::UniqueConnection);
}

void DesktopController::AddItem( const ItemData &item )
{
	m_desktopWindowData.Items.push_back(item);
	_AddItem(m_desktopWindowData.Items.last());
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

void DesktopController::RemveWindow()
{
	QFile::remove(m_savePath);
}

void DesktopController::Save()
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

//void DesktopController::ShowItems()
//{
//
//}
