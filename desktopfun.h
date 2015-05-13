#ifndef DESKTOPFUN_H
#define DESKTOPFUN_H

#include <QtGui/QWidget>
#include <QDialog>
#include <QSize>

#include "item.h"
#include "settingsDlg.h"


class WidgetMoveDragController;
class DesktopController;
class DesktopWindow : public QDialog
{
	Q_OBJECT

public:
	DesktopWindow(const QString &path, QWidget *parent = 0);
	~DesktopWindow();

	void Show();



private slots:
	void SlotActTriggered();
	void SlotAnimationFinish();
	void SlotSettingChanged(SettingData data);
private:
	void HideOrShowWindow(bool hide);
	void RemoveThisWindow();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	//void showEvent(QShowEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

private:
	friend class DesktopController;

	WidgetMoveDragController *m_moveDragController;
	DesktopController *m_desktopController;

	QAction	*m_actAddFile;
	QAction	*m_actAddDir;
	QAction	*m_actAnchorWindow;
	QAction	*m_actAnchorItem;
	QAction	*m_actAddNewWindow;
	QAction	*m_actRemoveNewWindow;
	QAction	*m_actSetting;

private:
	int		m_outside;
	// animation
	QPropertyAnimation	*m_animateWindow;
};



#endif // DESKTOPFUN_H
