#ifndef DESKTOPFUN_H
#define DESKTOPFUN_H

#include <QtGui/QWidget>
#include <QDialog>
#include <QSize>

#include "item.h"
#include "settingsDlg.h"


class WidgetMoveDragController;
class DesktopController;
class Controller;
class HandleWidget;
class DesktopWindow : public QDialog
{
	Q_OBJECT

public:
	DesktopWindow(const QString &path, QWidget *parent = 0);
	~DesktopWindow();

	void SetController(Controller *controller);
	void Show();

public:
	void SetIconSize(QSize size);
	QSize GetIconSize();

	void SetBgColor(QColor color);
	QColor GetBgColor();


private slots:
	void SlotActTriggered();
	void SlotOpenProcess(QString path);
	void SlotItemMenu(int menuID);
	void SlotItemHadMove();
	void SlotSaveDesk();
	void SlotAnimationFinish();
	void SlotSettingChanged(SettingData data);

private:
	void SetCanMove(bool canMove);
	//void SaveDesk();
	bool WillHideWindow();
	bool AnimationIsRun();
	void HideOrShowWindow(bool hide);
	void RemoveThisWindow();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void showEvent(QShowEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

private:
	WidgetMoveDragController *m_moveDragController;
	DesktopController *m_desktopController;

	Controller *m_controller;
//	QList<Item*> *m_items;

	QAction	*m_actAddFile;
	QAction	*m_actAddDir;
	QAction	*m_actAnchorWindow;
	QAction	*m_actAnchorItem;
	QAction	*m_actAddNewWindow;
	QAction	*m_actRemoveNewWindow;
	QAction	*m_actSetting;

private:
	bool	m_bWindowHadMove;
	DesktopWindowData	m_windowData;
	QSize	m_iconSize;
	
	int		m_outside;
private:
	// animation
	QPropertyAnimation	*m_animateWindow;
};



#endif // DESKTOPFUN_H
