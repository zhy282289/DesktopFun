#ifndef DATA_H
#define DATA_H

#include <QWidget>


QPixmap QueryExeIcon(const char *lpszExePath);
QPixmap QueryDesktopPixmap();

QString GetAppPath();
QString GetSettingPath();
QString GetPageSettingPath();

QString GetDefaultSaveDeskName();

HWND FindDesktopWnd();

void CreateNewDesktopWindow();
void CreateNewDesktopWindow(const QString &path);
#endif // DATA_H
