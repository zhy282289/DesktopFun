#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "item.h"
#include "desktopfun.h"
//
//class Controller : public QObject
//{
//	Q_OBJECT
//
//public:
//	Controller(QObject *parent = NULL);
//	~Controller();
//
//	QList<Item*> *Load(const QString &filePath, QWidget *parent);
//
//
//	QList<Item*> *GetItems();
//	
//	DesktopWindowData GetDesktopWindowData();
//	void SetDesktopWindowData(const DesktopWindowData &data);
//	void Save();
//	void Add(const ItemData &data, QWidget *parent);
//	void Remove(Item *item);
//	void RemoveAll();
//	void SetItemCanMove(bool move);
//	void SetItemBGColor(QColor color);
//	QString GetSavePath();
//
//private:
//	QList<Item*>	m_items;
//	DesktopWindowData m_desktopWindowData;
//	QString m_savePath;
//};
//
//Controller* GetController(QObject *parent = NULL);
#include <QRect>

class DesktopController : public QObject
{
	Q_OBJECT
public:
	explicit DesktopController(const QString path, QWidget *parent);

	void AddItem(const ItemData &item);
	void SetWindowMoveable(bool moveable);
	bool GetWindowMoveable();
	void SetItemMoveable(bool moveable);
	bool GetItemMoveable();
	QRect GetWindowGeometry();
	void RemveWindow();
	void Save();
private:
	bool Load(const QString &path);
	void _AddItem(ItemData &item);
	
private:
	QWidget *m_parent;
	QString m_savePath;
	DesktopWindowData m_desktopWindowData;
};


#endif // CONTROLLER_H
