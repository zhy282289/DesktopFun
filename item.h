#ifndef ITEM_H
#define ITEM_H

#include <QWidget>
#include <QRect>
#include <QPixmap>
#include <QMetaType>

#include <QLabel>

struct ItemData
{
	QString path;
	QRect rect;
	QPixmap pix;

};
Q_DECLARE_METATYPE(ItemData)
typedef QList<ItemData> ItemDatas;
QDataStream &operator<<(QDataStream &out, const ItemData &data);
QDataStream &operator>>(QDataStream &in, ItemData &data);

class ItemText;
class ItemPixmap;
class Item : public QWidget
{
	Q_OBJECT

public:
	Item(QWidget *parent);
	~Item();

	void SetData(const ItemData &data);
	ItemData GetData();
	void UpdatePostion();

	void SetCanMove(bool canMove);
	bool GetCanMove();
signals:
	void SigLeftMouseClicked(QString);
	void SigRightMouseClicked();
	void SigItemMenu(int);
	void SigItemHadMove();

private slots:
	void SlotActTriggered();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

	void contextMenuEvent(QContextMenuEvent *event);
private:
	ItemData	m_data;
	ItemText	*m_itemText;
	ItemPixmap	*m_itemPixmap;

	bool	m_bLeftBtnDown;
	bool	m_bWindowHadMove;
	QPoint  m_oldPoint;

	bool	m_bMouseEnter;
	bool	m_bItemCanMove;
	QAction *m_actRemove;

};


class ItemText : public QLabel
{
	Q_OBJECT
public:
	ItemText(QWidget *parent);

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
#endif // ITEM_H