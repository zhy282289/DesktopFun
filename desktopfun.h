#ifndef DESKTOPFUN_H
#define DESKTOPFUN_H

#include <QtGui/QWidget>
#include <QDialog>
#include <QSize>
#include "item.h"

struct DesktopWindowData
{
	QRect rect;
};
Q_DECLARE_METATYPE(DesktopWindowData)
QDataStream &operator<<(QDataStream &out, const DesktopWindowData &data);
QDataStream &operator>>(QDataStream &in, DesktopWindowData &data);


class Controller;
class DesktopWindow : public QDialog
{
	Q_OBJECT

public:
	DesktopWindow(QWidget *parent = 0);
	~DesktopWindow();

	void SetController(Controller *controller);
	void Show();

public:
	void SetIconSize(QSize size);
	QSize GetIconSize();

	void SetBgColor(QColor color);
	QColor GetBgColor();


private slots:
	void SlotOpenProcess(QString path);
	void SlotActTriggered();
	void SlotItemMenu(int menuID);
	void SlotItemHadMove();
	void SlotSaveDesk();

private:
	void HitSide( QMouseEvent *event);
	void MoveWindow(const QPoint &oldPoint, const QPoint &newPoint);
	void SetCanMove(bool canMove);
	void SaveDesk();
	bool WillHideWindow();
	bool AnimationIsRun();
	bool MouseInBorder(QPoint point);
	void HideOrShowWindow(bool hide);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void showEvent(QShowEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);

	void leaveEvent(QEvent *event);
	void enterEvent(QEvent *event);

private:
	bool	m_bLeftBtnDown;
	QPoint  m_oldPoint;
	Controller *m_controller;
	QList<Item*> *m_items;

	
	QAction	*m_actAddFile;
	QAction	*m_actAddDir;
	QAction	*m_actAnchor;
private:
	// settings
	bool	m_bMoveWindow;
	int		m_borderWidth;
	bool	m_bWindowHadMove;
	int		left;
	int		right;
	int		top;
	int		bottom;

	bool	m_bWindowCanMove;
	bool	m_bItemCanMove;
	QSize	m_iconSize;

	QColor	m_bgColor;

	QTimer	*m_saveDeskTimer;
	int		m_saveIntevalTime;

private:
	// animation
	QPropertyAnimation	*m_animateWindow;
};





#endif // DESKTOPFUN_H
