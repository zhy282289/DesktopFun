
#ifndef WIDGETDRAGCONTROLLER_H
#define WIDGETDRAGCONTROLLER_H



#include <QObject>
#include <QPoint>

class QWidget;
class QEvent;
class WidgetMoveDragController : public QObject
{
public:
	explicit WidgetMoveDragController(QObject *parent = 0);

	void SetBorderWidth(int borderWidget);
	void SetDragable(bool drag);
	void SetMoveable(bool move);
protected:
	bool eventFilter(QObject *obj, QEvent *event);
private:
	QWidget *m_dragWidget;
	int	m_borderWidth;
	bool m_bLeftMouseDown;
	bool m_bMoveWindow;
	bool m_bDragable;
	bool m_bMoveable;
	QPoint m_oldPointDrag;
	QPoint m_oldPointMove;

	bool m_left;
	bool m_top;
	bool m_right;
	bool m_bottom;
};















#endif