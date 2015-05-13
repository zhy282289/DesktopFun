#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QRect>
#include <QPixmap>
#include <QColor>

#include "item.h"
#include "desktopfun.h"

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
	void RemoveWindow();

	//
	QPixmap GetWindowBgPixmap();
	void SetWindowBgPixmap(const BGPixmap &pixmap);
	QColor GetWindowBgColor();
	void SetWindowBgColor(const QColor &color);
	bool IsWindowVisiable();
	void SetWindowVisiable(bool visiable);

public slots:
	void SaveState();
private:
	bool Load(const QString &path);
	void _AddItem(ItemData &item);
private slots:
	void DeleteLaterSlot(QString id);

private:
	DesktopWindow *m_parent;
	QString m_savePath;
	DesktopWindowData m_desktopWindowData;
};


#endif // CONTROLLER_H
