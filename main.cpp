#include "stdafx.h"
#include "desktopfun.h"
#include "controller.h"
#include "data.h"


void RegisterMetaTypeStream();
void LoadWindow();
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("gb18030"));


	LoadWindow();

	return a.exec();
}


void LoadWindow()
{
	QString path = GetPageSettingPath();
	QDir dir(path);
	QFileInfoList infoList = dir.entryInfoList(QStringList() << "*.desk", QDir::Files);
	for (int i = 0; i < infoList.size(); ++i)
	{
		const QString &filePath = infoList.at(i).absoluteFilePath();
		Controller *controller = new Controller;
		DesktopWindow *window = new DesktopWindow;
		controller->Load(filePath, window);
		window->SetController(controller);
		window->Show();

	}

	if (infoList.isEmpty())
	{
		Controller *controller = new Controller;
		DesktopWindow *window = new DesktopWindow;
		controller->Load("", window);
		window->SetController(controller);
		window->resize(500, 500);
		window->show();
	}

}

void RegisterMetaTypeStream()
{
	qRegisterMetaTypeStreamOperators<ItemData>("ItemData");
}