#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication
{
	Q_OBJECT

public:
	Application( int & argc, char ** argv);
	~Application();

protected:
	virtual bool notify ( QObject * receiver, QEvent * e );
};

#endif // APPLICATION_H
