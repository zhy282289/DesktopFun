#include "StdAfx.h"
#include "widgetmovedragcontroller.h"




WidgetMoveDragController::WidgetMoveDragController(QObject *parent)
	:QObject(parent)
	,m_borderWidth(2)
	,m_bLeftMouseDown(false)
	,m_bMoveWindow(false)
	,m_bMoveable(true)
	,m_bDragable(true)
{
	if (!parent-isWidgetType())
		Q_ASSERT(false);

	m_dragWidget = (QWidget*)parent;
	m_dragWidget->setMouseTracking(true);
	m_dragWidget->installEventFilter(this);
}


static QPoint g_point;
bool WidgetMoveDragController::eventFilter( QObject *obj, QEvent *event )
{
	switch (event->type())
	{
	case QEvent::MouseButtonPress:
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() == Qt::LeftButton)
			{
				m_bLeftMouseDown = true;
				m_oldPointDrag = mouseEvent->globalPos();
				m_oldPointMove = mouseEvent->pos();
				m_bMoveWindow = m_dragWidget->rect().adjusted(m_borderWidth, m_borderWidth, -m_borderWidth, -m_borderWidth).contains(m_oldPointMove);
			}
		}
		break;
	case QEvent::MouseButtonRelease:
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() == Qt::LeftButton)
				m_bLeftMouseDown = false;

		}
		break;
	case QEvent::MouseMove:
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
			if (m_bLeftMouseDown)
			{
				if(m_bMoveWindow && m_bMoveable)
					m_dragWidget->move(mouseEvent->globalPos() - m_oldPointMove);
				else if (m_bDragable)
				{
					QPoint newPoint = mouseEvent->globalPos();

					int dx = newPoint.x() - m_oldPointDrag.x();
					int dy = newPoint.y() - m_oldPointDrag.y();
					QRect rect = m_dragWidget->geometry();
					if (m_left)
						rect.setLeft(rect.x() + dx);
					if (m_top)
						rect.setTop(rect.y() + dy);
					if (m_right)
						rect.setRight(rect.x()+ rect.width()-1 + dx);
					if (m_bottom)
						rect.setBottom(rect.y() + rect.height()-1 + dy);
					m_dragWidget->setGeometry(rect);
					m_oldPointDrag = newPoint;
				}

			}
			else if (m_bDragable)
			{
				m_left = m_top = m_right = m_bottom = false;
				const int margin = m_borderWidth;
				QRect outRect = m_dragWidget->rect();
				QPoint pos = mouseEvent->pos();
				int x = pos.x();
				int y = pos.y();
				if (x < margin)
					m_left = true;
				if (y < margin)
					m_top = true;
				if (x > outRect.right()-margin)
					m_right = true;
				if (y > outRect.bottom()-margin)
					m_bottom = true;

				if (m_left && m_top)
					m_dragWidget->setCursor(Qt::SizeFDiagCursor);
				else if (m_left && m_bottom)
					m_dragWidget->setCursor(Qt::SizeBDiagCursor);
				else if (m_right && m_top)
					m_dragWidget->setCursor(Qt::SizeBDiagCursor);
				else if (m_right && m_bottom)
					m_dragWidget->setCursor(Qt::SizeFDiagCursor);
				else if (m_left || m_right)
					m_dragWidget->setCursor(Qt::SizeHorCursor);
				else if (m_top || m_bottom)
					m_dragWidget->setCursor(Qt::SizeVerCursor);
				else
					m_dragWidget->setCursor(Qt::ArrowCursor);
			}
		}
		break;

	}

	return false;
}

void WidgetMoveDragController::SetBorderWidth( int borderWidget )
{
	m_borderWidth = m_borderWidth;
}

void WidgetMoveDragController::SetDragable( bool drag )
{
	m_bDragable = drag;
}

void WidgetMoveDragController::SetMoveable( bool move )
{
	m_bMoveable = move;
}


