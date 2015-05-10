#include "stdafx.h"
#include "data.h"

#include "controller.h"
#include "desktopfun.h"

#include <shellapi.h>

QDataStream &operator<<(QDataStream &out, const ItemData &data)
{
	return out << data.id << data.path << data.pix << data.rect;
}

QDataStream &operator>>(QDataStream &in, ItemData &data)
{
	return in >> data.id >> data.path >> data.pix >> data.rect;
}

QDataStream &operator<<(QDataStream &out, const DesktopWindowData &data)
{
	return out << data.id << data.path << data.rect << data.bWindowCanMove << data.bItemCanMove << data.windowHadHide << data.color << data.bgPixmap.path << data.Items;
}
QDataStream &operator>>(QDataStream &in, DesktopWindowData &data)
{
	return in >> data.id >> data.path >> data.rect >> data.bWindowCanMove >> data.bItemCanMove >> data.windowHadHide >> data.color >> data.bgPixmap.path >> data.Items;
}




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
	if (!dir.exists(dirPath))
		dir.mkpath(dirPath);
	return QDir::toNativeSeparators(dirPath);
}


QString GetSongSettingPath()
{
	QString dirPath = GetSettingPath()+  "song"+ QDir::separator();
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
	//Controller *controller = new Controller;
	DesktopWindow *window = new DesktopWindow("");
	//controller->Load("", window);
	//window->SetController(controller);
	window->resize(400, 650);
	window->move(QPoint(350, 100));
	window->show();
}

void CreateNewDesktopWindow( const QString &path )
{
	const QString &filePath = path;
	//Controller *controller = new Controller;
	DesktopWindow *window = new DesktopWindow(filePath);
	//controller->Load(filePath, window);
	//window->SetController(controller);
	window->Show();
}

void OpenExplorerAndSelectFile( const QString &filePath )
{
	QString path = QDir::toNativeSeparators(filePath);
	QProcess precoss;
#ifdef Q_OS_WIN32
	QProcess::execute("explorer", QStringList() << "/e,/select," << path);
#else
	//QProcess::execute("open", QStringList() << "/e,/select,"<< filePath);
	string strPath = filePath.toLocal8Bit().data();
	NSString *fullPath = [NSString stringWithUTF8String:strPath.c_str()];
	return [[NSWorkspace sharedWorkspace] selectFile:fullPath inFileViewerRootedAtPath:nil];
#endif
}
