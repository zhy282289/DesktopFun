#include "stdafx.h"
#include "data.h"

#include "controller.h"
#include "desktopfun.h"

#include <shellapi.h>

QPixmap QueryExeIcon(const char *lpszExePath)
{
    SHFILEINFO FileInfo;
    DWORD_PTR dwRet = ::SHGetFileInfo(lpszExePath, 0, &FileInfo, sizeof(SHFILEINFO), SHGFI_ICON);
    // 目标文件不存在

	QPixmap pixmap;
    if (dwRet)
    {
        pixmap = QPixmap::fromWinHICON(FileInfo.hIcon);
		//::DestroyIcon(FileInfo.hIcon);
    }
    return pixmap;
}

QString GetSettingPath()
{
	QString dirPath = GetAppPath() + "settings"+ QDir::separator();
	QDir dir;
	dir.mkpath(dirPath);
	return QDir::toNativeSeparators(dirPath);
}

QString GetAppPath()
{
	return QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + QDir::separator());
}

QString GetPageSettingPath()
{
	QString dirPath = GetSettingPath()+  "page"+ QDir::separator();
	QDir dir;
	dir.mkpath(dirPath);
	return QDir::toNativeSeparators(dirPath);
}

static BOOL enumUserWindowsCB(HWND hwnd,LPARAM lParam)
{
	long wflags = GetWindowLong(hwnd, GWL_STYLE);
	if(!(wflags & WS_VISIBLE)) return TRUE;

	HWND sndWnd;
	if( !(sndWnd=FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL)) ) return TRUE;

	HWND targetWnd;
	if( !(targetWnd=FindWindowEx(sndWnd, NULL, "SysListView32", "FolderView")) ) return TRUE;

	HWND* resultHwnd = (HWND*)lParam;
	*resultHwnd = targetWnd;

	return FALSE;
}

HWND FindDesktopWnd()
{
	//HWND resultHwnd = NULL;
	//EnumWindows((WNDENUMPROC)enumUserWindowsCB, (LPARAM)&resultHwnd);
	//return resultHwnd;

	return FindWindowEx(::FindWindowEx(::FindWindow(TEXT("Progman"), TEXT("Program Manager")), NULL, TEXT("SHELLDLL_DefView"), TEXT("")), NULL, TEXT("SysListView32"), TEXT("FolderView"));
}

QPixmap *g_desktopPixmap = NULL;
QPixmap QueryDesktopPixmap()
{
	if (g_desktopPixmap == NULL)
	{
		g_desktopPixmap = new QPixmap;
		*g_desktopPixmap = QPixmap::grabWindow(FindDesktopWnd());
	}

	return *g_desktopPixmap;
}

QString GetDefaultSaveDeskName()
{
	QString path = GetPageSettingPath();
	QDir dir(path);
	QFileInfoList infoList = dir.entryInfoList(QStringList() << "*.desk", QDir::Files);
	
	QString defaultName("default_0.desk");
	
	int num = 1;
	while(1)
	{
		bool exitsDefaultName = false;
		for (int i = 0; i < infoList.size(); ++i)
		{
			const QString &filePath = infoList.at(i).absoluteFilePath();
			if (filePath.contains(defaultName))
			{
				exitsDefaultName = true;
				break;
			}

		}
		if (exitsDefaultName)
			defaultName = QString("default_%1.desk").arg(num++);
		else
			break;
	}
	

	return defaultName;

}

void CreateNewDesktopWindow()
{
	Controller *controller = new Controller;
	DesktopWindow *window = new DesktopWindow;
	controller->Load("", window);
	window->SetController(controller);
	window->resize(400, 650);
	window->move(QPoint(350, 100));
	window->show();
}

void CreateNewDesktopWindow( const QString &path )
{
	const QString &filePath = path;
	Controller *controller = new Controller;
	DesktopWindow *window = new DesktopWindow;
	controller->Load(filePath, window);
	window->SetController(controller);
	window->Show();
}
