#ifndef DESKTOPFUN_H
#define DESKTOPFUN_H

#include <QtGui/QWidget>
#include <QDialog>
#include <QSize>
#include "item.h"

struct DesktopWindowData
{
	QRect   rect;
	bool	bWindowCanMove;
	bool	bItemCanMove;
	bool	windowHadHide;
	DesktopWindowData()
	{
		bWindowCanMove = true;
		bItemCanMove = true;
		windowHadHide = false;
	}
};

Q_DECLARE_METATYPE(DesktopWindowData)
QDataStream &operator<<(QDataStream &out, const DesktopWindowData &data);
QDataStream &operator>>(QDataStream &in, DesktopWindowData &data);


class Controller;
class HandleWidget;
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
	void SlotAnimationFinish();

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
	void focusOutEvent(QFocusEvent *event);
	void focusInEvent(QFocusEvent *event);
	void leaveEvent(QEvent *event);
	void enterEvent(QEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

private:
	bool	m_bLeftBtnDown;
	QPoint  m_oldPoint;
	Controller *m_controller;
	QList<Item*> *m_items;

	
	QAction	*m_actAddFile;
	QAction	*m_actAddDir;
	QAction	*m_actAnchor;
	QAction	*m_actAddNewWindow;
	QAction	*m_actRemoveNewWindow;
	QAction	*m_actSetting;

private:
	// settings
	bool	m_bMoveWindow;
	int		m_borderWidth;
	bool	m_bWindowHadMove;
	int		left;
	int		right;
	int		top;
	int		bottom;

	DesktopWindowData	m_windowData;
	//bool	m_bWindowCanMove;
	//bool	m_bItemCanMove;

	QSize	m_iconSize;

	QColor	m_bgColor;

	QTimer	*m_saveDeskTimer;
	int		m_saveIntevalTime;
	//bool	m_windowHadHide;

	int		m_outside;
private:
	// animation
	QPropertyAnimation	*m_animateWindow;
};


class HandleWidget : public QWidget
{
	Q_OBJECT
public:
	HandleWidget(QWidget *parent = NULL);

	void Move(QPoint point);
	void Hide();

signals:
	void clicked();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);

	QColor m_bgColor;
};


#endif // DESKTOPFUN_H
