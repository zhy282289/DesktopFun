#ifndef DATA_H
#define DATA_H

#include <QWidget>



struct BGPixmap
{
	QPixmap bgPixmap;
	QString path;
};
struct SettingData
{
	QColor color;
	BGPixmap bgPix;
};


// һ������ͼ������
struct ItemData
{
	QString id;			// id
	QString path;	// ͼ��·��
	QRect rect;		// ͼ���С
	QPixmap pix;	// ͼ��
};

Q_DECLARE_METATYPE(ItemData)
typedef QList<ItemData> ItemDatas;
QDataStream &operator<<(QDataStream &out, const ItemData &data);
QDataStream &operator>>(QDataStream &in, ItemData &data);




// һ�����ڵ���������
struct DesktopWindowData
{
	QString id;				// id
	QString path;			// desktop pixmap path
	QRect   rect;			// window position and size
	bool	bWindowCanMove;	// window can move
	bool	bItemCanMove;	// item can move
	bool	windowHadHide;	// has hide
	QColor	color;			// bgcolor
	BGPixmap bgPixmap;		// bg

	QList<ItemData>	Items;	// all items
	DesktopWindowData()
	{
		bWindowCanMove = true;
		bItemCanMove = true;
		windowHadHide = false;
		color = QColor(250, 20, 80, 150);
	}
};

Q_DECLARE_METATYPE(DesktopWindowData)
QDataStream &operator<<(QDataStream &out, const DesktopWindowData &data);
QDataStream &operator>>(QDataStream &in, DesktopWindowData &data);



QPixmap QueryExeIcon(const char *lpszExePath);
QPixmap QueryDesktopPixmap();

QString GetAppPath();
QString GetSettingPath();
QString GetPageSettingPath();
QString GetSongSettingPath();

QString GetDefaultSaveDeskName();


HWND FindDesktopWnd();
HWND FindDesktopWndXP();
HWND FindDesktopWndWin7();

void CreateNewDesktopWindow();
void CreateNewDesktopWindow(const QString &path);



static const int g_iconSize = 80;

void OpenExplorerAndSelectFile(const QString &filePath);





#endif // DATA_H
