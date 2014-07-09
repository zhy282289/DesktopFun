#include "stdafx.h"
#include "application.h"

Application::Application(int & argc, char ** argv)
	: QApplication(argc, argv)
{

}

Application::~Application()
{

}

bool Application::notify( QObject * receiver, QEvent * e )
{
	return QApplication::notify(receiver, e);
}
