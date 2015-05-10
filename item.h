#ifndef ITEM_H
#define ITEM_H

#include <QWidget>
#include <QRect>
#include <QPixmap>
#include <QMetaType>

#include <QLabel>


#include "data.h"
//struct ItemData
//{
//	QString path;
//	QRect rect;
//	QPixmap pix;
//	bool bItemCanMove;
//	ItemData()
//	{
//		bItemCanMove = true;
//	}
//};
//Q_DECLARE_METATYPE(ItemData)
//typedef QList<ItemData> ItemDatas;
//QDataStream &operator<<(QDataStream &out, const ItemData &data);
//QDataStream &operator>>(QDataStream &in, ItemData &data);

class ItemText;
class ItemPixmap;
//class Item : public QWidget
//{
//	Q_OBJECT
//
//public:
//	Item(QWidget *parent);
//	~Item();
//
//	void SetData(const ItemData &data);
//	ItemData GetData();
//	void UpdatePostion();
//
//	void SetCanMove(bool canMove);
//	bool GetCanMove();
//
//	void SetBGColor(QColor color);
//
//signals:
//	void SigLeftMouseClicked(QString);
//	void SigRightMouseClicked();
//	void SigItemMenu(int);
//	void SigItemHadMove();
//
//	private slots:
//		void SlotActTriggered();
//
//protected:
//	void paintEvent(QPaintEvent *event);
//	void mousePressEvent(QMouseEvent *event);
//	void mouseReleaseEvent(QMouseEvent *event);
//	void mouseMoveEvent(QMouseEvent *event);
//
//	void enterEvent(QEvent *event);
//	void leaveEvent(QEvent *event);
//
//	void contextMenuEvent(QContextMenuEvent *event);
//private:
//	ItemData	m_data;
//	ItemText	*m_itemText;
//	ItemPixmap	*m_itemPixmap;
//
//	bool	m_bLeftBtnDown;
//	bool	m_bWindowHadMove;
//	QPoint  m_oldPoint;
//
//	bool	m_bMouseEnter;
//	QAction *m_actRemove;
//	QAction *m_actLocateDirectory;
//
//	QColor  m_bgColor;
//};


class ItemText : public QLabel
{
	Q_OBJECT
public:
	ItemText(QWidget *parent);

	void SetText(const QString &text);
private:

};

class ItemPixmap : public QLabel
{
	Q_OBJECT
public:
	ItemPixmap(QWidget *parent);

	void SetPixmap(const QPixmap &pixmap);

protected:
	void paintEvent(QPaintEvent *event);

private:
	QPixmap	m_pixmap;
};


class ItemEx : public QWidget
{
	Q_OBJECT

public:
	ItemEx(ItemData *data, QWidget *parent);
	~ItemEx();


	const ItemData& GetData();

	void UpdatePostion();

	void SetCanMove(bool *canMove);
	bool GetCanMove();

	void SetBGColor(QColor color);

signals:
	//void SigLeftMouseClicked(QString);
	//void SigRightMouseClicked();
	//void SigItemMenu(int);
	void SigItemHadMove();

//private slots:
//	void SlotActTriggered();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

	void contextMenuEvent(QContextMenuEvent *event);
private:
	ItemData	*m_data;
	ItemText	*m_itemText;
	ItemPixmap	*m_itemPixmap;

	bool    *m_canMove;

	bool	m_bLeftBtnDown;
	bool	m_bWindowHadMove;
	QPoint  m_oldPoint;

	bool	m_bMouseEnter;
	//QAction *m_actRemove;
	//QAction *m_actLocateDirectory;

	QColor  m_bgColor;
};


#endif // ITEM_H
