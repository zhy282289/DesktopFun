#ifndef ITEM_H
#define ITEM_H

#include <QWidget>
#include <QRect>
#include <QPixmap>
#include <QMetaType>
#include <QLabel>

#include "data.h"

class ItemText;
class ItemPixmap;
class ItemEx : public QWidget
{
	Q_OBJECT

public:
	ItemEx(ItemData *data, QColor bgColor, bool *canMove, QWidget *parent);
	~ItemEx();

	void SetGeometry();

signals:
	void WindowStateChanged();
	void DeleteLater(QString uuid);

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
	QColor  m_bgColor;
};

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


#endif // ITEM_H
