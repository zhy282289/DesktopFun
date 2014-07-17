#include "stdafx.h"
#include "desktopfun.h"
#include "controller.h"
#include "data.h"
#include "application.h"

#include "settingsdlg.h"

//Q_IMPORT_PLUGIN(qcncodecs)

void RegisterMetaTypeStream();
void LoadWindow();
int main(int argc, char *argv[])
{
	Application a(argc, argv);
	
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("gb18030"));


	QString appStyleSheet = "QPushButton{border:1px solid gray; background-color:rgb(250,198,255);min-width:82px;min-height:22px}"
		"QPushButton:hover{background-color:rgb(250,220,255);}"
		"QMenu{background-color:rgb(255,225,226);min-width:200px}"
		"QMenu:item{background:rgb(255,225,226);padding: 8px 20px 8px 20px;width:180px;}"
		"QMenu:item:selected,QMenu:item:checked:selected{ background-color: rgb(255,172,251);}"
		"QMenu:item:checked{ background-color: rgb(255,210,255);}"
		"QTableView{background-color:rgb(255,210,255); selection-background-color:rgb(255,172,251);}"
		;
	a.setStyleSheet(appStyleSheet);

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
		CreateNewDesktopWindow();
	}

}

void RegisterMetaTypeStream()
{
	qRegisterMetaTypeStreamOperators<ItemData>("ItemData");
}