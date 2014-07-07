#include "stdafx.h"
#include "controller.h"
#include "data.h"


Controller::Controller(QObject *parent)
	: QObject(parent)
{

}

Controller::~Controller()
{

}

QList<Item*> *Controller::Load( const QString &filePath, QWidget *parent)
{
	m_savePath = filePath;
	
	QFile file(m_savePath);
	if (file.open(QIODevice::ReadOnly))
	{
		QList<ItemData> itemDatas;
		QDataStream stream(&file);
		stream >> m_desktopWindowData;
		stream >> itemDatas;
		for (int i = 0; i < itemDatas.size(); ++i)
		{
			Add(itemDatas.at(i), parent);
		}
	}

	return &m_items;
}

void Controller::Save()
{
	if (m_savePath.isEmpty())
	{
		m_savePath = GetPageSettingPath() + GetDefaultSaveDeskName();
	}
	QFile file(m_savePath);
	if (file.open(QIODevice::WriteOnly))
	{
		QList<ItemData> itemDatas;
		for (int i = 0; i < m_items.size(); ++i)
		{
			ItemData itemData = m_items.at(i)->GetData();
			itemData.rect = m_items.at(i)->geometry();
			itemDatas.push_back(itemData);
		}

		QDataStream stream(&file);
		stream << m_desktopWindowData;
		stream << itemDatas;
	}
}
void Controller::Add(const ItemData &data, QWidget *parent)
{
	Item *w = new Item(parent);
	w->SetData(data);
	m_items.push_back(w);
	w->UpdatePostion();

	connect(w, SIGNAL(SigLeftMouseClicked(QString)), parent, SLOT(SlotOpenProcess(QString)), Qt::UniqueConnection);
	connect(w, SIGNAL(SigItemMenu(int)), parent, SLOT(SlotItemMenu(int)), Qt::UniqueConnection);
	connect(w, SIGNAL(SigItemHadMove()), parent, SLOT(SlotItemHadMove()), Qt::UniqueConnection);
}

//void Controller::Add( const QString &path, QWidget *parent )
//{
//	ItemData data;
//	data.rect = QRect(50, 50, 80, 80);
//	data.path = QDir::toNativeSeparators(path);
//	QByteArray pathArray = data.path.toLocal8Bit();
//	data.pix = QueryExeIcon(pathArray.data());
//	Item *w = new Item(parent);
//	w->SetData(data);
//	m_items.push_back(w);
//	w->UpdatePostion();
//
//	connect(w, SIGNAL(SigLeftMouseClicked(QString)), parent, SLOT(SlotOpenProcess(QString)), Qt::UniqueConnection);
//	connect(w, SIGNAL(SigItemMenu(int)), parent, SLOT(SlotItemMenu(int)), Qt::UniqueConnection);
//}

void Controller::Remove( Item *item )
{
	m_items.removeOne(item);
	item->setParent(NULL);
	item->deleteLater();
}


QList<Item*> * Controller::GetItems()
{
	return &m_items;
}

DesktopWindowData Controller::GetDesktopWindowData()
{
	return m_desktopWindowData;
}

void Controller::SetDesktopWindowData( const DesktopWindowData &data )
{
	m_desktopWindowData = data;
}

void Controller::SetItemCanMove( bool move )
{
	for (int i = 0; i < m_items.size(); ++i)
	{
		m_items.at(i)->SetCanMove(move);
	}
}


Controller *g_controller = NULL;
Controller* GetController( QObject *parent /*= NULL*/ )
{
	if (g_controller == NULL)
	{
		g_controller = new Controller(parent);
	}
	return g_controller;
}
